################################################################################
# The MIT License
#
# Copyright (c) 2021, Prominence AI, Inc.
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
sys.path.insert(0, "../../")
from dsl import *

uri_file = "../../test/streams/sample_1080p_h264.mp4"

# Filespecs for the Primary GIE and IOU Trcaker
primary_infer_config_file = '../../test/configs/config_infer_primary_nano.txt'
primary_model_engine_file = '../../test/models/Primary_Detector_Nano/resnet10.caffemodel_b8_gpu0_fp16.engine'
tracker_config_file = '../../test/configs/iou_config.txt'

PGIE_CLASS_ID_VEHICLE = 0
PGIE_CLASS_ID_BICYCLE = 1
PGIE_CLASS_ID_PERSON = 2
PGIE_CLASS_ID_ROADSIGN = 3

MIN_OBJECTS = 3
MAX_OBJECTS = 8

TILER_WIDTH = DSL_DEFAULT_STREAMMUX_WIDTH
TILER_HEIGHT = DSL_DEFAULT_STREAMMUX_HEIGHT
WINDOW_WIDTH = 1280
WINDOW_HEIGHT = 720

INCLUSION_AREA = 0
EXCLUSION_AREA = 1

g_capture_instance = 0

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
        dsl_main_loop_quit()
 
## 
# Function to be called on XWindow Delete event
## 
def xwindow_delete_event_handler(client_data):
    print('delete window event')
    dsl_main_loop_quit()

## 
# Function to be called on End-of-Stream (EOS) event
## 
def eos_event_listener(client_data):
    print('Pipeline EOS event')
    dsl_main_loop_quit()

## 
# Function to be called on every change of Pipeline state
## 
def state_change_listener(old_state, new_state, client_data):
    print('previous state = ', old_state, ', new state = ', new_state)
    if new_state == DSL_STATE_PLAYING:
        dsl_pipeline_dump_to_dot('pipeline', "state-playing")

## 	
# Objects of this class will be used as "client_data" for the 
# player_termination_event_listener() callback below.	
# defines a class of all component names associated with a single RTSP Source. 	
# The names are derived from the unique Source name	
##	
class PlayerComponents:	
    def __init__(self, suffix):
        self.player = 'player-' + suffix
        self.source = 'source-' + suffix
        self.sink = 'sink-' + suffix

## 
# Function to be called on Player termination event
## 
def player_termination_event_listener(client_data):
    print(' ***  Display Image Complete  *** ')

    # cast the C void* client_data back to a py_object pointer and deref	
    playerComponents = cast(client_data, POINTER(py_object)).contents.value	

    # delete the player and its two components
    dsl_player_delete(playerComponents.player)
#    dsl_component_delete_many(components=[
#        playerComponents.source, playerComponents.sink, None])

    # reset the Trigger so that a new image can be captured.
    print(dsl_return_value_to_string(dsl_ode_trigger_reset('person-enter-area-trigger')))


def capture_complete_listener(capture_info_ptr, client_data):
    print(' ***  Object Capture Complete  *** ')
    
    global g_capture_instance
    g_capture_instance += 1
    
    capture_info = capture_info_ptr.contents
    print('capture_id: ', capture_info.capture_id)
    print('filename:   ', capture_info.filename)
    print('dirpath:    ', capture_info.dirpath)
    print('width:      ', capture_info.width)
    print('height:     ', capture_info.height)

    # Create the unique player component names for this capture instance
    playerComponents = PlayerComponents(str(g_capture_instance))
    
    print(playerComponents.source)

    # New Image Source using the dirpath and filename
    
    retval = dsl_source_image_new(
        name = playerComponents.source, 
        file_path = capture_info.dirpath + '/' + capture_info.filename, 
        is_live = False,
        fps_n = 10,
        fps_d = 1,
        timeout = 5)
    if retval != DSL_RETURN_SUCCESS:
        return
    
    # New Overlay Sink
    retval = dsl_sink_overlay_new(
        name = playerComponents.sink, 
        overlay_id = 1, 
        display_id = 0, 
        depth = 0,
        offset_x = 50, 
        offset_y = 50, 
        width = capture_info.width*2, 
        height = capture_info.height*2)
    if retval != DSL_RETURN_SUCCESS:
        return

    # New Media Player using the File Source and Window Sink
    retval = dsl_player_new(
        name = playerComponents.player,
        source = playerComponents.source, 
        sink = playerComponents.sink)
    if retval != DSL_RETURN_SUCCESS:
        return

    # Add the Termination listener callback to the Player
    # Pass the playerComponents (i.e. names) as client data
    retval = dsl_player_termination_event_listener_add(playerComponents.player,
        client_listener=player_termination_event_listener, client_data=playerComponents)
    if retval != DSL_RETURN_SUCCESS:
        return

    # Play the Player until end-of-stream (EOS)
    retval = dsl_player_play(playerComponents.player)
    if retval != DSL_RETURN_SUCCESS:
        return
    

def main(args):

    # Since we're not using args, we can Let DSL initialize GST on first call
    while True:
    
        # This example demonstrates the use of a Polygon Area for Inclusion 
        # or Exlucion critera for ODE occurrence. Change the variable below to try each.
        
        area_type = INCLUSION_AREA
        
        #```````````````````````````````````````````````````````````````````````````````````

        # Create a Hide-Area Action to hide all Display Text and Bounding Boxes
        retval = dsl_ode_action_hide_new('hide-both', text=True, border=True)
        if retval != DSL_RETURN_SUCCESS:
            break

        # Create an Any-Class Occurrence Trigger for our Hide Action
        retval = dsl_ode_trigger_occurrence_new('any-occurrence-trigger', source=DSL_ODE_ANY_SOURCE,
            class_id=DSL_ODE_ANY_CLASS, limit=DSL_ODE_TRIGGER_LIMIT_NONE)
        if retval != DSL_RETURN_SUCCESS:
            break
        retval = dsl_ode_trigger_action_add('any-occurrence-trigger', action='hide-both')
        if retval != DSL_RETURN_SUCCESS:
            break

        retval = dsl_display_type_rgba_color_new('opaque-red', red=1.0, green=0.0, blue=0.0, alpha=0.3)
        if retval != DSL_RETURN_SUCCESS:
            break
            
        # Create a new  Action used to fill a bounding box with the opaque red color
        retval = dsl_ode_action_fill_object_new('fill-action', color='opaque-red')
        if retval != DSL_RETURN_SUCCESS:
            break

        # create a list of X,Y coordinates defining the points of the Polygon.
        # Polygon can have a minimum of 3, maximum of 8 points (sides)
        coordinates = [dsl_coordinate(365,600), dsl_coordinate(580,620), 
            dsl_coordinate(600, 770), dsl_coordinate(180,750)]
            
        # Create the Polygon display type 
        retval = dsl_display_type_rgba_polygon_new('polygon1', 
            coordinates=coordinates, num_coordinates=len(coordinates), border_width=4, color='opaque-red')
        if retval != DSL_RETURN_SUCCESS:
            break
            
        # create the ODE inclusion area to use as criteria for ODE occurrence
        if area_type == INCLUSION_AREA:
            retval = dsl_ode_area_inclusion_new('polygon-area', polygon='polygon1', 
                show=True, bbox_test_point=DSL_BBOX_POINT_SOUTH)    
            if retval != DSL_RETURN_SUCCESS:
                break
        else:
            retval = dsl_ode_area_exclusion_new('polygon-area', polygon='polygon1', 
                show=True, bbox_test_point=DSL_BBOX_POINT_SOUTH)    
            if retval != DSL_RETURN_SUCCESS:
                break

        # New Occurrence Trigger, filtering on PERSON class_id, 
        # and with no limit on the number of occurrences
        retval = dsl_ode_trigger_occurrence_new('person-in-area-trigger',
            source = DSL_ODE_ANY_SOURCE,
            class_id = PGIE_CLASS_ID_PERSON, 
            limit = DSL_ODE_TRIGGER_LIMIT_NONE)
        if retval != DSL_RETURN_SUCCESS:
            break
            
        retval = dsl_ode_trigger_area_add('person-in-area-trigger', area='polygon-area')
        if retval != DSL_RETURN_SUCCESS:
            break
        
        retval = dsl_ode_trigger_action_add('person-in-area-trigger', action='fill-action')
        if retval != DSL_RETURN_SUCCESS:
            break


        # New Occurrence Trigger, filtering on PERSON class_id, for our capture object action
        # with a limit of one which will be reset in the capture-complete callback
        retval = dsl_ode_trigger_instance_new('person-enter-area-trigger', 
            source = DSL_ODE_ANY_SOURCE,
            class_id = PGIE_CLASS_ID_PERSON, 
            limit = DSL_ODE_TRIGGER_LIMIT_ONE)
        if retval != DSL_RETURN_SUCCESS:
            break

        # Using the same Inclusion area as the New Occurrence Trigger
        retval = dsl_ode_trigger_area_add('person-enter-area-trigger', area='polygon-area')
        if retval != DSL_RETURN_SUCCESS:
            break

        # Create a new Capture Action to capture the object to jpeg image, and save to file. 
        retval = dsl_ode_action_capture_object_new('person-capture-action', outdir="./")
        if retval != DSL_RETURN_SUCCESS:
            break
        
        # Add the capture complete listener function to the action
        retval = dsl_ode_action_capture_complete_listener_add('person-capture-action', 
            capture_complete_listener, None)

        retval = dsl_ode_trigger_action_add('person-enter-area-trigger', 
            action='person-capture-action')
        if retval != DSL_RETURN_SUCCESS:
            break


        #```````````````````````````````````````````````````````````````````````````````````````````````````````````````
        
        # New ODE Handler to handle all ODE Triggers with their Areas and Actions    
        retval = dsl_pph_ode_new('ode-handler')
        if retval != DSL_RETURN_SUCCESS:
            break
        retval = dsl_pph_ode_trigger_add_many('ode-handler', triggers=[
            'any-occurrence-trigger', 
            'person-in-area-trigger', 
            'person-enter-area-trigger',
            None])
        if retval != DSL_RETURN_SUCCESS:
            break
        
        
        ############################################################################################
        #
        # Create the remaining Pipeline components
        
        # New URI File Source using the filespec defined above
        retval = dsl_source_uri_new('uri-source', uri_file, False, 0, 0, 0)
        if retval != DSL_RETURN_SUCCESS:
            break

        # New Primary GIE using the filespecs above with interval = 0
        retval = dsl_gie_primary_new('primary-gie', primary_infer_config_file, primary_model_engine_file, 1)
        if retval != DSL_RETURN_SUCCESS:
            break

        # New KTL Tracker, setting max width and height of input frame
        retval = dsl_tracker_iou_new('iou-tracker', tracker_config_file, 480, 272)
        if retval != DSL_RETURN_SUCCESS:
            break

        # New Tiled Display, setting width and height, use default cols/rows set by source count
        retval = dsl_tiler_new('tiler', TILER_WIDTH, TILER_HEIGHT)
        if retval != DSL_RETURN_SUCCESS:
            break
 
         # Add our ODE Pad Probe Handler to the Sink pad of the Tiler
        retval = dsl_tiler_pph_add('tiler', handler='ode-handler', pad=DSL_PAD_SINK)
        if retval != DSL_RETURN_SUCCESS:
            break

        # New OSD with clock and text enabled... using default values.
        retval = dsl_osd_new('on-screen-display', True, True)
        if retval != DSL_RETURN_SUCCESS:
            break

        # New Window Sink, 0 x/y offsets and same dimensions as Tiled Display
        retval = dsl_sink_window_new('window-sink', 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT)
        if retval != DSL_RETURN_SUCCESS:
            break

        # Add all the components to our pipeline
        retval = dsl_pipeline_new_component_add_many('pipeline', 
            ['uri-source', 'primary-gie', 'iou-tracker', 'tiler', 'on-screen-display', 'window-sink', None])
        if retval != DSL_RETURN_SUCCESS:
            break

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

    # Cleanup all DSL/GST resources
    dsl_delete_all()
    
if __name__ == '__main__':
    sys.exit(main(sys.argv))