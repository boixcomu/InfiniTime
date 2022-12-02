#pragma once
#include <cstdint>
#include <chrono>
#include "displayapp/screens/Screen.h"
#include "systemtask/SystemTask.h"
#include <lvgl/src/lv_core/lv_style.h>
#include <lvgl/src/lv_core/lv_obj.h>
#include <components/motion/MotionController.h>

namespace Pinetime {
    namespace Controllers {
        class HeartRateController;
    }
    namespace Applications {
        namespace Screens {

            class Twente : public Screen {
            public:
                Twente(DisplayApp* app, Controllers::HeartRateController& HeartRateController, System::SystemTask& systemTask, Controllers::MotionController& motionController);  
                ~Twente() override;

                void Refresh() override;  
                void OnStartStopEvent_T(lv_event_t event);
            
            private:
                //controllers used
                Controllers::HeartRateController& heartRateController;
                Pinetime::System::SystemTask& systemTask;
                Controllers::MotionController& motionController;
                

                //chart object display raw PPG
                lv_obj_t* chart; //chart
                lv_chart_series_t* ser1; //line in chart
                
                //label to display X Y Z BPM
                lv_obj_t* label; 
                
                //button start stop
                void UpdateStartStopButton(bool isRunning);
                lv_obj_t* btn_startStop;
                lv_obj_t* label_startStop;

                lv_task_t* taskRefresh;

            };

        }
        
    } // namespace Applications
    
}// namespace Pinetime