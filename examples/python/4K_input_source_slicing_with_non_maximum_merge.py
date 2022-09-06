################################################################################
# The MIT License
#
# Copyright (c) 2019-2021, Prominence AI, Inc.
#
# Permission is hereby granted, free of charge, to any person obtaining a
# copy of this software and associated documentation files (the "Software"),
# to deal in the Software without restriction, including without limitation
# the rights to use, copy, modify, merge, publish, distribute, sublicense,
# and/or sell copies of the Software, and to permit persons to whom the
# Software is furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
# THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
# FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
# DEALINGS IN THE SOFTWARE.
################################################################################

#!/usr/bin/env python

import sys
from dsl import *

image_file = "../../test/streams/4K-image.jpg"

# Config file used with the Preprocessor
preproc_config_file = \
    '../../test/configs/config_preprocess_4k_input_slicing.txt'
    
# Filespecs for the Primary GIE and IOU Trcaker
primary_infer_config_file = \
    '/opt/nvidia/deepstream/deepstream/sources/apps/sample_apps/deepstream-preprocess-test/config_infer.txt'

# IMPORTANT! ensure that the model-engine was generated with the config from the Preprocessing example
#  - apps/sample_apps/deepstream-preprocess-test/config_infer.txt
primary_model_engine_file = \
    '/opt/nvidia/deepstream/deepstream/samples/models/Primary_Detector/resnet10.caffemodel_b3_gpu0_fp16.engine'
tracker_config_file = \
    '/opt/nvidia/deepstream/deepstream/samples/configs/deepstream-app/config_tracker_IOU.yml'

PGIE_CLASS_ID_VEHICLE = 0
PGIE_CLASS_ID_BICYCLE = 1
PGIE_CLASS_ID_PERSON = 2
PGIE_CLASS_ID_ROADSIGN = 3

## 
# Function to be called on XWindow KeyRelease event
## 
def xwindow_key_event_handler(key_string, client_data):
    print('key released = ', key_string)
    if key_string.upper() == 'P':
        dsl_pipeline_pause('pipeline')
    elif key_string.upper() == 'R':
        dsl_pipeline_play('pipeline')
    elif key_string.upper() == 'Q' or key_string == '' or key_string == '':
        dsl_pipeline_stop('pipeline')
        dsl_main_loop_quit()
 
## 
# Function to be called on XWindow Delete event
## 
def xwindow_delete_event_handler(client_data):
    print('delete window event')
    dsl_pipeline_stop('pipeline')
    dsl_main_loop_quit()

## 
# Function to be called on End-of-Stream (EOS) event
## 
def eos_event_listener(client_data):
    print('Pipeline EOS event')

#    dsl_pipeline_stop('pipeline')
#    dsl_main_loop_quit()

## 
# Function to be called on every change of Pipeline state
## 
def state_change_listener(old_state, new_state, client_data):
    print('previous state = ', old_state, ', new state = ', new_state)
    if new_state == DSL_STATE_PLAYING:
        dsl_pipeline_dump_to_dot('pipeline', "state-playing")

## 
# Function to be called on Object Capture (and file-save) complete
## 
def capture_complete_listener(capture_info_ptr, client_data):
    print(' ***  Object Capture Complete  *** ')
    
    capture_info = capture_info_ptr.contents
    print('capture_id: ', capture_info.capture_id)
    print('filename:   ', capture_info.filename)
    print('dirpath:    ', capture_info.dirpath)
    print('width:      ', capture_info.width)
    print('height:     ', capture_info.height)

def main(args):

    # Since we're not using args, we can Let DSL initialize GST on first call
    while True:

        # --------------------------------------------------------------------------------
        # Create a new Capture Action to capture the Frame to jpeg image, and save to file. 
        retval = dsl_ode_action_label_customize_new('customize-label-action', 
            content_types=[DSL_METRIC_OBJECT_CLASS, DSL_METRIC_OBJECT_TRACKING_ID, 
            DSL_METRIC_OBJECT_CONFIDENCE_INFERENCE], size=3)
        if retval != DSL_RETURN_SUCCESS:
            break

        # New Occurrence Trigger, filtering on PERSON class_id, 
        # and with no limit on the number of occurrences
        retval = dsl_ode_trigger_occurrence_new('person-occurrence-trigger-1',
            source = DSL_ODE_ANY_SOURCE,
            class_id = PGIE_CLASS_ID_PERSON, 
            limit = DSL_ODE_TRIGGER_LIMIT_NONE)
        if retval != DSL_RETURN_SUCCESS:
            break

        retval = dsl_ode_trigger_action_add('person-occurrence-trigger-1', 
            action='customize-label-action')
        if retval != DSL_RETURN_SUCCESS:
            break
        # New ODE Handler to handle all ODE Triggers with their Areas and Actions    
        retval = dsl_pph_ode_new('ode-handler-1')
        if retval != DSL_RETURN_SUCCESS:
            break
        retval = dsl_pph_ode_trigger_add_many('ode-handler-1', triggers=[
            'person-occurrence-trigger-1',None])
        if retval != DSL_RETURN_SUCCESS:
            break

        # --------------------------------------------------------------------------------
        # Create a new Capture Action to capture the Frame to jpeg image, and save to file. 
        retval = dsl_ode_action_capture_frame_new('frame-capture-action',
            outdir = "./",
            annotate = False)
        if retval != DSL_RETURN_SUCCESS:
            break

        # New Occurrence Trigger, filtering on PERSON class_id, 
        # and with no limit on the number of occurrences
        retval = dsl_ode_trigger_occurrence_new('person-occurrence-trigger-2',
            source = DSL_ODE_ANY_SOURCE,
            class_id = PGIE_CLASS_ID_PERSON, 
            limit = DSL_ODE_TRIGGER_LIMIT_ONE)
        if retval != DSL_RETURN_SUCCESS:
            break

        retval = dsl_ode_trigger_action_add('person-occurrence-trigger-2', 
            action='frame-capture-action')
        if retval != DSL_RETURN_SUCCESS:
            break
        # New ODE Handler to handle all ODE Triggers with their Areas and Actions    
        retval = dsl_pph_ode_new('ode-handler-2')
        if retval != DSL_RETURN_SUCCESS:
            break
        retval = dsl_pph_ode_trigger_add_many('ode-handler-2', triggers=[
            'person-occurrence-trigger-2',None])
        if retval != DSL_RETURN_SUCCESS:
            break
            
        # --------------------------------------------------------------------------------

        # New URI File Source using the filespec defined above
#        retval = dsl_source_image_new('image-source', image_file)
#        if retval != DSL_RETURN_SUCCESS:
#            break

        retval = dsl_source_image_stream_new('image-source', file_path=image_file,
            is_live=False, fps_n=10, fps_d=1, timeout=0)
        if retval != DSL_RETURN_SUCCESS:
            break

        # New Preprocessor component using the config filespec defined above.
        retval = dsl_preproc_new('preprocessor', preproc_config_file)
        if retval != DSL_RETURN_SUCCESS:
            break
        
        # New Primary GIE using the filespecs above with interval = 0
        retval = dsl_infer_gie_primary_new('primary-gie', 
            primary_infer_config_file, primary_model_engine_file, 0)
        if retval != DSL_RETURN_SUCCESS:
            break

        # **** IMPORTANT! for best performace we explicity set the GIE's batch-size 
        # to the number of ROI's defined in the Preprocessor configuraton file.
        retval = dsl_infer_batch_size_set('primary-gie', 3)
        if retval != DSL_RETURN_SUCCESS:
            break

        # **** IMPORTANT! we must set the input-meta-tensor setting to true when
        # using the preprocessor, otherwise the GIE will use its own preprocessor.
        retval = dsl_infer_gie_tensor_meta_settings_set('primary-gie',
            input_enabled=True, output_enabled=False);

        # New IOU Tracker, setting max width and height of input frame
        retval = dsl_tracker_iou_new('iou-tracker', tracker_config_file, 640, 368)
        if retval != DSL_RETURN_SUCCESS:
            break

        # New OSD with text, clock and bbox display all enabled. 
        retval = dsl_osd_new('on-screen-display', 
            text_enabled=True, clock_enabled=True, bbox_enabled=True, mask_enabled=False)
        if retval != DSL_RETURN_SUCCESS:
            break
            
        retval = dsl_osd_pph_add('on-screen-display', 'ode-handler-1', DSL_PAD_SINK)
        if retval != DSL_RETURN_SUCCESS:
            break

        retval = dsl_osd_pph_add('on-screen-display', 'ode-handler-2', DSL_PAD_SRC)
        if retval != DSL_RETURN_SUCCESS:
            break

        # New Window Sink, 0 x/y offsets and same dimensions as Tiled Display
        retval = dsl_sink_window_new('window-sink', 0, 0, 
            width=DSL_STREAMMUX_4K_UHD_WIDTH, height=DSL_STREAMMUX_4K_UHD_WIDTH)
        if retval != DSL_RETURN_SUCCESS:
            break

        # Add all the components to our pipeline
        retval = dsl_pipeline_new_component_add_many('pipeline', components=[
            'image-source', 'preprocessor', 'primary-gie', 'iou-tracker',
            'on-screen-display', 'window-sink', None])
        if retval != DSL_RETURN_SUCCESS:
            break

        # Update the Pipeline's streammuxer dimensions 
        retval = dsl_pipeline_streammux_dimensions_set('pipeline', 
            width=DSL_STREAMMUX_4K_UHD_WIDTH, height= DSL_STREAMMUX_4K_UHD_HEIGHT)
        
        # Add the XWindow event handler functions defined above
        retval = dsl_pipeline_xwindow_key_event_handler_add("pipeline", xwindow_key_event_handler, None)
        if retval != DSL_RETURN_SUCCESS:
            break
        retval = dsl_pipeline_xwindow_delete_event_handler_add("pipeline", xwindow_delete_event_handler, None)
        if retval != DSL_RETURN_SUCCESS:
            break

        ## Add the listener callback functions defined above
        retval = dsl_pipeline_state_change_listener_add('pipeline', state_change_listener, None)
        if retval != DSL_RETURN_SUCCESS:
            break
        retval = dsl_pipeline_eos_listener_add('pipeline', eos_event_listener, None)
        if retval != DSL_RETURN_SUCCESS:
            break

        # Play the pipeline
        retval = dsl_pipeline_play('pipeline')
        if retval != DSL_RETURN_SUCCESS:
            break

        dsl_main_loop_run()
        retval = DSL_RETURN_SUCCESS
        break

    # Print out the final result
    print(dsl_return_value_to_string(retval))

    dsl_pipeline_delete_all()
    dsl_component_delete_all()

if __name__ == '__main__':
    sys.exit(main(sys.argv))