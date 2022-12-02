#include "displayapp/screens/Twente.h"
#include <lvgl/lvgl.h>

#include <components/heartrate/HeartRateController.h>

#include "displayapp/DisplayApp.h"
#include "displayapp/InfiniTimeTheme.h"

using namespace Pinetime::Applications::Screens;

namespace {
    void btnStartStopEventHandler(lv_obj_t* obj, lv_event_t event) {
    auto* screen = static_cast<Twente*>(obj->user_data);
    screen->OnStartStopEvent_T(event);
  }
}

Twente::Twente( Pinetime::Applications::DisplayApp* app,
                Controllers::HeartRateController& heartRateController,
                System::SystemTask& systemTask,
                Controllers::MotionController& motionController)
    : Screen(app), heartRateController {heartRateController}, systemTask {systemTask}, motionController {motionController} {
    bool isHrRunning = heartRateController.State() != Controllers::HeartRateController::States::Stopped;
//chart to show PPG raw
    chart = lv_chart_create(lv_scr_act(), NULL);
    lv_obj_set_size(chart, 240, 240);
    lv_obj_align(chart, NULL, LV_ALIGN_IN_TOP_MID, 0, 0);
    lv_chart_set_type(chart, LV_CHART_TYPE_LINE); /*Show lines and points too*/
    // lv_chart_set_series_opa(chart, LV_OPA_70);                            /*Opacity of the data series*/
    // lv_chart_set_series_width(chart, 4);                                  /*Line width and point radious*/

    lv_chart_set_range(chart, -1100, 1100);
    lv_chart_set_update_mode(chart, LV_CHART_UPDATE_MODE_SHIFT);
    lv_chart_set_point_count(chart, 200);

    //data series
    ser1 = lv_chart_add_series(chart, Colors::orange);

    lv_chart_init_points(chart, ser1, 0);
    lv_chart_refresh(chart); /*Required after direct set*/    

//label with X Y Z and BPM
    label = lv_label_create(lv_scr_act(), NULL);
    lv_label_set_text_fmt(label, "X #FF0000 %d# Y #00B000 %d# Z #FFFF00 %d# bpm #FFB000 %d#", 0, 0, 0, 0);
    lv_label_set_align(label, LV_LABEL_ALIGN_CENTER);
    lv_obj_align(label, NULL, LV_ALIGN_IN_TOP_MID, 0, 10);
    lv_label_set_recolor(label, true);

//button start stop
    btn_startStop = lv_btn_create(lv_scr_act(), nullptr);
    btn_startStop->user_data = this;
    lv_obj_set_height(btn_startStop, 50);
    lv_obj_set_event_cb(btn_startStop, btnStartStopEventHandler);
    lv_obj_align(btn_startStop, nullptr, LV_ALIGN_IN_BOTTOM_MID, 0, 0);

    label_startStop = lv_label_create(btn_startStop, nullptr);
    UpdateStartStopButton(isHrRunning);
    
    if (isHrRunning)
        //not allaw screen to go off if it's running
        systemTask.PushMessage(Pinetime::System::Messages::DisableSleeping);

    //copied from heartRate.cpp still need to know what it does
    taskRefresh = lv_task_create(RefreshTaskCallback, 100, LV_TASK_PRIO_MID, this);
}

Twente::~Twente(){
    lv_task_del(taskRefresh);
    lv_obj_clean(lv_scr_act());
    systemTask.PushMessage(Pinetime::System::Messages::EnableSleeping);   
}

void Twente::Refresh(){
//update top label
    auto state = heartRateController.State();
    switch (state) {
        case Controllers::HeartRateController::States::NoTouch:
        case Controllers::HeartRateController::States::NotEnoughData:
            // case Controllers::HeartRateController::States::Stopped:
            lv_label_set_text_fmt(label,
                        "X #FF0000 %d# Y #00B000 %d# Z #FFFF00 %d# bpm #FFB000 000#",
                        motionController.X() / 0x10,
                        motionController.Y() / 0x10,
                        motionController.Z() / 0x10);
            break;
        default:
            lv_label_set_text_fmt(label,
                        "X #FF0000 %d# Y #00B000 %d# Z #FFFF00 %d# bpm #FFB000 %03d#",
                        motionController.X() / 0x10,
                        motionController.Y() / 0x10,
                        motionController.Z() / 0x10,
                        heartRateController.HeartRate());
        }
    
    lv_obj_align(label, NULL, LV_ALIGN_IN_TOP_MID, 0, 10);
//update chart ?? how to extract the PPG data?

}

void Twente::OnStartStopEvent_T(lv_event_t event){
    if (event == LV_EVENT_CLICKED) {
        if (heartRateController.State() == Controllers::HeartRateController::States::Stopped) {
            heartRateController.Start();
            UpdateStartStopButton(heartRateController.State() != Controllers::HeartRateController::States::Stopped);
            systemTask.PushMessage(Pinetime::System::Messages::DisableSleeping);
            //lv_obj_set_style_local_text_color(label_hr, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, Colors::highlight);
        } else {
            heartRateController.Stop();
            UpdateStartStopButton(heartRateController.State() != Controllers::HeartRateController::States::Stopped);
            systemTask.PushMessage(Pinetime::System::Messages::EnableSleeping);
            //lv_obj_set_style_local_text_color(label_hr, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, Colors::lightGray);
        }
    }
}

void Twente::UpdateStartStopButton(bool isRunning){
    if (isRunning)
        lv_label_set_text_static(label_startStop, "Stop");
    else
        lv_label_set_text_static(label_startStop, "Start");

}