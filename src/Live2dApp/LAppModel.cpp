﻿/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#include "LAppModel.hpp"
#include <fstream>
#include <vector>
#include <CubismModelSettingJson.hpp>
#include <Motion/CubismMotion.hpp>
#include <Physics/CubismPhysics.hpp>
#include <CubismDefaultParameterId.hpp>
#include <Rendering/OpenGL/CubismRenderer_OpenGLES2.hpp>
#include <Utils/CubismString.hpp>
#include <Id/CubismIdManager.hpp>
#include <Motion/CubismMotionQueueEntry.hpp>
#include "LAppDefine.hpp"
#include "LAppPal.hpp"
#include "LAppTextureManager.hpp"
#include "LAppDelegate.hpp"

#include"app_log.h"

using namespace Live2D::Cubism::Framework;
using namespace Live2D::Cubism::Framework::DefaultParameterId;
using namespace LAppDefine;

namespace {
    csmByte* CreateBuffer(const csmChar* path, csmSizeInt* size)
    {
        if (DebugLogEnable)
        {
            LAppPal::PrintLog("[APP]create buffer: %s ", path);
        }
        return LAppPal::LoadFileAsBytes(path, size);
    }

    void DeleteBuffer(csmByte* buffer, const csmChar* path = "")
    {
        if (DebugLogEnable)
        {
            LAppPal::PrintLog("[APP]delete buffer: %s", path);
        }
        LAppPal::ReleaseBytes(buffer);
    }
}

LAppModel::LAppModel()
    : CubismUserModel()
    , _modelSetting(NULL)
    , _userTimeSeconds(0.0f)
{
    // setlocale(LC_ALL, "en_US.UTF-8");
    if (DebugLogEnable)
    {
        _debugMode = true;
    }

    _idParamAngleX = CubismFramework::GetIdManager()->GetId(ParamAngleX);
    _idParamAngleY = CubismFramework::GetIdManager()->GetId(ParamAngleY);
    _idParamAngleZ = CubismFramework::GetIdManager()->GetId(ParamAngleZ);
    _idParamBodyAngleX = CubismFramework::GetIdManager()->GetId(ParamBodyAngleX);
    _idParamEyeBallX = CubismFramework::GetIdManager()->GetId(ParamEyeBallX);
    _idParamEyeBallY = CubismFramework::GetIdManager()->GetId(ParamEyeBallY);

}

LAppModel::~LAppModel()
{
    _renderBuffer.DestroyOffscreenFrame();

    ReleaseMotions();
    ReleaseExpressions();
    if(_modelSetting)
    {
        for (csmInt32 i = 0; i < _modelSetting->GetMotionGroupCount(); i++)
        {
            const csmChar* group = _modelSetting->GetMotionGroupName(i);
            ReleaseMotionGroup(group);
        }
        delete(_modelSetting);
    }
}

bool LAppModel::LoadAssets(const csmChar* dir, const csmChar* fileName)
{
    _modelHomeDir = dir;

    if (_debugMode)
    {
        LAppPal::PrintLog("[APP]load model setting: %s", fileName);
    }

    csmSizeInt size;


    const csmString path = csmString(dir) + fileName;
    APP_LOG_DEBUG(path.GetRawString());
    csmByte* buffer = CreateBuffer(path.GetRawString(), &size);
    if(buffer == NULL)
    {
        return false;
    }
    ICubismModelSetting* setting = new CubismModelSettingJson(buffer, size);
    DeleteBuffer(buffer, path.GetRawString());

    SetupModel(setting);

    CreateRenderer();

    SetupTextures();
    return true;
}


bool LAppModel::SetupModel(ICubismModelSetting* setting)
{
    _updating = true;
    _initialized = false;

    _modelSetting = setting;

    csmByte* buffer;
    csmSizeInt size;

    //Cubism Model
    if (strcmp(_modelSetting->GetModelFileName(), "") != 0)
    {
        csmString path = _modelSetting->GetModelFileName();
        path = _modelHomeDir + path;

        if (_debugMode)
        {
            LAppPal::PrintLog("[APP]create model: %s", setting->GetModelFileName());
        }

        buffer = CreateBuffer(path.GetRawString(), &size);
        if(buffer==NULL)
        {
            return false;
        }
        LoadModel(buffer, size);
        DeleteBuffer(buffer, path.GetRawString());
    }

    //Expression
    if (_modelSetting->GetExpressionCount() > 0)
    {
        const csmInt32 count = _modelSetting->GetExpressionCount();
        for (csmInt32 i = 0; i < count; i++)
        {
            csmString name = _modelSetting->GetExpressionName(i);
            csmString path = _modelSetting->GetExpressionFileName(i);
            path = _modelHomeDir + path;

            buffer = CreateBuffer(path.GetRawString(), &size);
            if(buffer == NULL)
            {
                return false;
            }

            ACubismMotion* motion = LoadExpression(buffer, size, name.GetRawString());

            if (_expressions[name] != NULL)
            {
                ACubismMotion::Delete(_expressions[name]);
                _expressions[name] = NULL;
            }
            _expressions[name] = motion;

            DeleteBuffer(buffer, path.GetRawString());
        }
    }

    //Physics
    if (strcmp(_modelSetting->GetPhysicsFileName(), "") != 0)
    {
        csmString path = _modelSetting->GetPhysicsFileName();
        path = _modelHomeDir + path;

        buffer = CreateBuffer(path.GetRawString(), &size);
        if(buffer)
        {
            LoadPhysics(buffer, size);
            DeleteBuffer(buffer, path.GetRawString());
        }
    }

    //Pose
    if (strcmp(_modelSetting->GetPoseFileName(), "") != 0)
    {
        csmString path = _modelSetting->GetPoseFileName();
        path = _modelHomeDir + path;

        buffer = CreateBuffer(path.GetRawString(), &size);
        if(buffer)
        {
            LoadPose(buffer, size);
            DeleteBuffer(buffer, path.GetRawString());
        }
    }

    //EyeBlink
    if (_modelSetting->GetEyeBlinkParameterCount() > 0)
    {
        _eyeBlink = CubismEyeBlink::Create(_modelSetting);
    }

    //Breath
    {
        _breath = CubismBreath::Create();

        csmVector<CubismBreath::BreathParameterData> breathParameters;

        breathParameters.PushBack(CubismBreath::BreathParameterData(_idParamAngleX, 0.0f, 15.0f, 6.5345f, 0.5f));
        breathParameters.PushBack(CubismBreath::BreathParameterData(_idParamAngleY, 0.0f, 8.0f, 3.5345f, 0.5f));
        breathParameters.PushBack(CubismBreath::BreathParameterData(_idParamAngleZ, 0.0f, 10.0f, 5.5345f, 0.5f));
        breathParameters.PushBack(CubismBreath::BreathParameterData(_idParamBodyAngleX, 0.0f, 4.0f, 15.5345f, 0.5f));
        breathParameters.PushBack(CubismBreath::BreathParameterData(CubismFramework::GetIdManager()->GetId(ParamBreath), 0.5f, 0.5f, 3.2345f, 0.5f));

        _breath->SetParameters(breathParameters);
    }

    //UserData
    if (strcmp(_modelSetting->GetUserDataFile(), "") != 0)
    {
        csmString path = _modelSetting->GetUserDataFile();
        path = _modelHomeDir + path;
        buffer = CreateBuffer(path.GetRawString(), &size);
        if(buffer)
        {
            LoadUserData(buffer, size);
            DeleteBuffer(buffer, path.GetRawString());
        }
    }

    // EyeBlinkIds
    {
        csmInt32 eyeBlinkIdCount = _modelSetting->GetEyeBlinkParameterCount();
        for (csmInt32 i = 0; i < eyeBlinkIdCount; ++i)
        {
            _eyeBlinkIds.PushBack(_modelSetting->GetEyeBlinkParameterId(i));
        }
    }

    // LipSyncIds
    {
        csmInt32 lipSyncIdCount = _modelSetting->GetLipSyncParameterCount();
        LAppPal::PrintLog("[APP]  lipSyncIdCount: %d",lipSyncIdCount);
        for (csmInt32 i = 0; i < lipSyncIdCount; ++i)
        {
            _lipSyncIds.PushBack(_modelSetting->GetLipSyncParameterId(i));
        }
    }

    //Layout
    csmMap<csmString, csmFloat32> layout;
    _modelSetting->GetLayoutMap(layout);
    _modelMatrix->SetupFromLayout(layout);

    _model->SaveParameters();

    for (csmInt32 i = 0; i < _modelSetting->GetMotionGroupCount(); i++)
    {
        const csmChar* group = _modelSetting->GetMotionGroupName(i);
        PreloadMotionGroup(group);
    }


    //
    PreloadHitAreaGroup();

    _motionManager->StopAllMotions();

    _updating = false;
    _initialized = true;
    return true;
}

void LAppModel::PreloadHitAreaGroup()
{
    for (csmInt32 i = 0; i < _modelSetting->GetHitAreasCount(); i++)
    {
        const csmChar* area_name = _modelSetting->GetHitAreaName(i);
        LAppPal::PrintLog("[APP] getHitAreaName  :  =>  %s", area_name);
        hitArea_single temp;
        temp.name = area_name;
        temp.touch_id=_modelSetting->GetHitAreaId(i);
        temp.motion = const_cast<Csm::csmChar*>(_modelSetting->GetHitAreaMotion(i));
        temp.order = _modelSetting->GetHitAreaOrder(i);
        _hitAreas.PushBack(temp);
        LAppPal::PrintLog("[APP] PreloadHitAreaGroup: %s %s %d %d ", area_name, temp.motion, temp.order,temp.touch_id->GetString());
    }
}


void LAppModel::PreloadMotionGroup(const csmChar* group)
{
    const csmInt32 count = _modelSetting->GetMotionCount(group);


    for (csmInt32 i = 0; i < count; i++)
    {
        //ex) idle_0
        csmString name = Utils::CubismString::GetFormatedString("%s_%d", group, i);
        csmString path = _modelSetting->GetMotionFileName(group, i);
        path = _modelHomeDir + path;

        if (_debugMode)
        {
            LAppPal::PrintLog("[APP]load motion: %s => [%s_%d] ", path.GetRawString(), group, i);
        }

        csmByte* buffer;
        csmSizeInt size;
        buffer = CreateBuffer(path.GetRawString(), &size);
        if(buffer)
        {
            CubismMotion* tmpMotion = static_cast<CubismMotion*>(LoadMotion(buffer, size, name.GetRawString()));
            csmFloat32 fadeTime = _modelSetting->GetMotionFadeInTimeValue(group, i);
            if (fadeTime >= 0.0f)
            {
                tmpMotion->SetFadeInTime(fadeTime);
            }

            fadeTime = _modelSetting->GetMotionFadeOutTimeValue(group, i);
            if (fadeTime >= 0.0f)
            {
                tmpMotion->SetFadeOutTime(fadeTime);
            }
            tmpMotion->SetEffectIds(_eyeBlinkIds, _lipSyncIds);

            if (_motions[name] != NULL)
            {
                ACubismMotion::Delete(_motions[name]);
            }
            _motions[name] = tmpMotion;

            DeleteBuffer(buffer, path.GetRawString());
        }
    }
}

void LAppModel::ReleaseMotionGroup(const csmChar* group) const
{
    const csmInt32 count = _modelSetting->GetMotionCount(group);
    for (csmInt32 i = 0; i < count; i++)
    {
        csmString voice = _modelSetting->GetMotionSoundFileName(group, i);
        if (strcmp(voice.GetRawString(), "") != 0)
        {
            csmString path = voice;
            path = _modelHomeDir + path;
        }
    }
}

/**
* @brief すべてのモーションデータの解放
*
* すべてのモーションデータを解放する。
*/
void LAppModel::ReleaseMotions()
{
    for (csmMap<csmString, ACubismMotion*>::const_iterator iter = _motions.Begin(); iter != _motions.End(); ++iter)
    {
        ACubismMotion::Delete(iter->Second);
    }

    _motions.Clear();
}

/**
* @brief すべての表情データの解放
*
* すべての表情データを解放する。
*/
void LAppModel::ReleaseExpressions()
{
    for (csmMap<csmString, ACubismMotion*>::const_iterator iter = _expressions.Begin(); iter != _expressions.End(); ++iter)
    {
        ACubismMotion::Delete(iter->Second);
    }

    _expressions.Clear();
}

void LAppModel::Update()
{
    const csmFloat32 deltaTimeSeconds = LAppPal::GetDeltaTime();
    _userTimeSeconds += deltaTimeSeconds;

    _dragManager->Update(deltaTimeSeconds);
    _dragX = _dragManager->GetX();
    _dragY = _dragManager->GetY();

    // モーションによるパラメータ更新の有無
    csmBool motionUpdated = false;

    //-----------------------------------------------------------------
    _model->LoadParameters(); // 前回セーブされた状態をロード
    if (_motionManager->IsFinished())
    {
        // モーションの再生がない場合、待機モーションの中からランダムで再生する
        StartRandomMotion(MotionGroupIdle, PriorityIdle);
    }
    else
    {
        motionUpdated = _motionManager->UpdateMotion(_model, deltaTimeSeconds); // モーションを更新
    }
    _model->SaveParameters(); // 状態を保存
    //-----------------------------------------------------------------

    // まばたき
    if (!motionUpdated)
    {
        if (_eyeBlink != NULL)
        {
            // メインモーションの更新がないとき
            _eyeBlink->UpdateParameters(_model, deltaTimeSeconds); // 目パチ
        }
    }

    if (_expressionManager != NULL)
    {
        _expressionManager->UpdateMotion(_model, deltaTimeSeconds); // 表情でパラメータ更新（相対変化）
    }

    //ドラッグによる変化
    //ドラッグによる顔の向きの調整
    _model->AddParameterValue(_idParamAngleX, _dragX * 30); // -30から30の値を加える
    _model->AddParameterValue(_idParamAngleY, _dragY * 30);
    _model->AddParameterValue(_idParamAngleZ, _dragX * _dragY * -30);

    //ドラッグによる体の向きの調整
    _model->AddParameterValue(_idParamBodyAngleX, _dragX * 10); // -10から10の値を加える

    //ドラッグによる目の向きの調整
    _model->AddParameterValue(_idParamEyeBallX, _dragX); // -1から1の値を加える
    _model->AddParameterValue(_idParamEyeBallY, _dragY);

    // 呼吸など
    if (_breath != NULL)
    {
        _breath->UpdateParameters(_model, deltaTimeSeconds);
    }

    // 物理演算の設定
    if (_physics != NULL)
    {
        _physics->Evaluate(_model, deltaTimeSeconds);
    }

    // リップシンクの設定
    if (_lipSync)
    {
        // リアルタイムでリップシンクを行う場合、システムから音量を取得して0〜1の範囲で値を入力します。
        csmFloat32 value = 0.0f;
        // LAppPal::PrintLog("[APP]  lipSync: %d",_lipSyncIds.GetSize());
        // 状態更新/RMS値取得
        _wavFileHandler.Update(deltaTimeSeconds);
        value = _wavFileHandler.GetRms();

        for (csmUint32 i = 0; i < _lipSyncIds.GetSize(); ++i)
        {
            _model->AddParameterValue(_lipSyncIds[i], value, 0.8f);
        }
    }

    // ポーズの設定
    if (_pose != NULL)
    {
        _pose->UpdateParameters(_model, deltaTimeSeconds);
    }

    //嘴巴 hansolo
    _model->AddParameterValue(CubismFramework::GetIdManager()->GetId(csmString("ParamMouthOpenY").GetRawString()), mouse_user_value, 0.8f);


    _model->Update();

}

void LAppModel::set_lipSynclValue(csmFloat32 value)
{
    mouse_user_value = value;
}

CubismMotionQueueEntryHandle LAppModel::StartMotion(const csmChar* group, csmInt32 no, csmInt32 priority, ACubismMotion::FinishedMotionCallback onFinishedMotionHandler)
{
    if (priority == PriorityForce)
    {
        _motionManager->SetReservePriority(priority);
    }
    else if (!_motionManager->ReserveMotion(priority))
    {
        if (_debugMode)
        {
            LAppPal::PrintLog("[APP]can't start motion.");
        }
        return InvalidMotionQueueEntryHandleValue;
    }

    const csmString fileName = _modelSetting->GetMotionFileName(group, no);
    APP_LOG_DEBUG(fileName.GetRawString());
    //ex) idle_0
    csmString name = Utils::CubismString::GetFormatedString("%s_%d", group, no);
    CubismMotion* motion = static_cast<CubismMotion*>(_motions[name.GetRawString()]);
    csmBool autoDelete = false;

    if (motion == NULL)
    {
        csmString path = fileName;
        path = _modelHomeDir + path;

        csmByte* buffer;
        csmSizeInt size;
        buffer = CreateBuffer(path.GetRawString(), &size);
        if(buffer)
        {
            motion = static_cast<CubismMotion*>(LoadMotion(buffer, size, NULL, onFinishedMotionHandler));
            csmFloat32 fadeTime = _modelSetting->GetMotionFadeInTimeValue(group, no);
            if (fadeTime >= 0.0f)
            {
                motion->SetFadeInTime(fadeTime);
            }

            fadeTime = _modelSetting->GetMotionFadeOutTimeValue(group, no);
            if (fadeTime >= 0.0f)
            {
                motion->SetFadeOutTime(fadeTime);
            }
            motion->SetEffectIds(_eyeBlinkIds, _lipSyncIds);
            autoDelete = true; // 終了時にメモリから削除

            DeleteBuffer(buffer, path.GetRawString());
        }
    }
    else
    {
        motion->SetFinishedMotionHandler(onFinishedMotionHandler);
    }

    //voice
    csmString voice = _modelSetting->GetMotionSoundFileName(group, no);
    if (strcmp(voice.GetRawString(), "") != 0)
    {
        csmString path = voice;
        path = _modelHomeDir + path;
        _wavFileHandler.Start(path);
    }

    if (_debugMode)
    {
        LAppPal::PrintLog("[APP]start motion: [%s_%d]", group, no);
    }
    return  _motionManager->StartMotionPriority(motion, autoDelete, priority);
}

CubismMotionQueueEntryHandle LAppModel::StartRandomMotion(const csmChar* group, csmInt32 priority, ACubismMotion::FinishedMotionCallback onFinishedMotionHandler)
{
    if (_modelSetting->GetMotionCount(group) == 0)
    {
        return InvalidMotionQueueEntryHandleValue;
    }

    csmInt32 no = rand() % _modelSetting->GetMotionCount(group);

    return StartMotion(group, no, priority, onFinishedMotionHandler);
}

void LAppModel::DoDraw()
{
    if (_model == NULL)
    {
        return;
    }

    GetRenderer<Rendering::CubismRenderer_OpenGLES2>()->DrawModel();
}

void LAppModel::Draw(CubismMatrix44& matrix)
{
    if (_model == NULL)
    {
        return;
    }

    matrix.MultiplyByMatrix(_modelMatrix);

    GetRenderer<Rendering::CubismRenderer_OpenGLES2>()->SetMvpMatrix(&matrix);

    DoDraw();
}


csmBool LAppModel::HitTest_Custom(Csm::csmFloat32 x, Csm::csmFloat32 y,Csm::csmString &hit_motion)
{
    // 透明时没有命中检测
    if (_opacity < 1)
    {
        return false;
    }
    // 获取可触摸区域的数量
    const csmInt32 count = _modelSetting->GetHitAreasCount();
    LAppPal::PrintLog("[APP]hit area==>GetHitAreasCount : [%d]", count);

    for(csmInt32 i = 0; i < count; i++)
    {
         // 比较命中区域名称
        const CubismIdHandle drawID = _modelSetting->GetHitAreaId(i);

        if(IsHit(drawID, x, y)){
            LAppPal::PrintLog("[APP]hit area==>GetHitAreaName : [%s]",_modelSetting->GetHitAreaName(i));
            LAppPal::PrintLog("[APP]hit area==>GetHitAreaId : [%d]",drawID);

            for (csmInt32 j = 0; j < count;j++){
                const Csm::csmChar *cur_name = _hitAreas[j].name.GetRawString();
                if(strcmp(cur_name,_modelSetting->GetHitAreaName(i))==0){
                    LAppPal::PrintLog("[APP]hit area find fit motion==>  : [%s]",_hitAreas[j].motion);
                    hit_motion = Csm::csmString(_hitAreas[j].motion);
                    return true;
                }
            }
        }
    }
    return false;
}

csmBool LAppModel::HitTest(const csmChar* hitAreaName, csmFloat32 x, csmFloat32 y)
{
    // 透明時は当たり判定なし。
    // 透明时没有命中检测
    if (_opacity < 1)
    {
        return false;
    }
    // 获取可触摸区域的数量
    const csmInt32 count = _modelSetting->GetHitAreasCount();
    LAppPal::PrintLog("[APP]hit area==>GetHitAreasCount : [%d]", count);
    for (csmInt32 i = 0; i < count; i++)
    {
         // 比较命中区域名称
        const CubismIdHandle drawID = _modelSetting->GetHitAreaId(i);

        if(IsHit(drawID, x, y)){
            LAppPal::PrintLog("[APP]hit area==>GetHitAreaName : [%s]",_modelSetting->GetHitAreaName(i));
            LAppPal::PrintLog("[APP]hit area==>GetHitAreaId : [%d]",drawID);
            return true;
        }
        if (strcmp(_modelSetting->GetHitAreaName(i), hitAreaName) == 0)
        {
            // 获取命中区域的绘制 ID
            // LAppPal::PrintLog("[APP]hit area==>GetHitAreaId : [%d]", drawID);
            return IsHit(drawID, x, y);
        }
    }
    // 如果没有找到匹配的命中区域，返回 false
    return false; // 存在しない場合はfalse
}

void LAppModel::SetExpression(const csmChar* expressionID)
{
    ACubismMotion* motion = _expressions[expressionID];
    if (_debugMode)
    {
        LAppPal::PrintLog("[APP]expression: [%s]", expressionID);
    }

    if (motion != NULL)
    {
        _expressionManager->StartMotionPriority(motion, false, PriorityForce);
    }
    else
    {
        if (_debugMode) LAppPal::PrintLog("[APP]expression[%s] is null ", expressionID);
    }
}

void LAppModel::SetRandomExpression()
{
    if (_expressions.GetSize() == 0)
    {
        return;
    }

    csmInt32 no = rand() % _expressions.GetSize();
    csmMap<csmString, ACubismMotion*>::const_iterator map_ite;
    csmInt32 i = 0;
    for (map_ite = _expressions.Begin(); map_ite != _expressions.End(); map_ite++)
    {
        if (i == no)
        {
            csmString name = (*map_ite).First;
            SetExpression(name.GetRawString());
            return;
        }
        i++;
    }
}

void LAppModel::ReloadRenderer()
{
    DeleteRenderer();

    CreateRenderer();

    SetupTextures();
}

void LAppModel::SetupTextures()
{
    for (csmInt32 modelTextureNumber = 0; modelTextureNumber < _modelSetting->GetTextureCount(); modelTextureNumber++)
    {
        // テクスチャ名が空文字だった場合はロード・バインド処理をスキップ
        if (strcmp(_modelSetting->GetTextureFileName(modelTextureNumber), "") == 0)
        {
            continue;
        }

        //OpenGLのテクスチャユニットにテクスチャをロードする
        csmString texturePath = _modelSetting->GetTextureFileName(modelTextureNumber);
        texturePath = _modelHomeDir + texturePath;

        LAppTextureManager::TextureInfo* texture = LAppDelegate::GetInstance()->GetTextureManager()->CreateTextureFromPngFile(texturePath.GetRawString());
        if(texture != NULL)
        {
            const csmInt32 glTextueNumber = texture->id;
            //OpenGL
            GetRenderer<Rendering::CubismRenderer_OpenGLES2>()->BindTexture(modelTextureNumber, glTextueNumber);
        }
    }

#ifdef PREMULTIPLIED_ALPHA_ENABLE
    GetRenderer<Rendering::CubismRenderer_OpenGLES2>()->IsPremultipliedAlpha(true);
#else
    GetRenderer<Rendering::CubismRenderer_OpenGLES2>()->IsPremultipliedAlpha(false);
#endif

}

void LAppModel::MotionEventFired(const csmString& eventValue)
{
    CubismLogInfo("%s is fired on LAppModel!!", eventValue.GetRawString());
}

Csm::Rendering::CubismOffscreenFrame_OpenGLES2& LAppModel::GetRenderBuffer()
{
    return _renderBuffer;
}
