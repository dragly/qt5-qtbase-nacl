# Build libppapi_cpp with source code from the NaCl SDK.
PPAPI_CPP_SOURCE= $$(NACL_SDK_ROOT)/src/ppapi_cpp
SOURCES += \
    $${PPAPI_CPP_SOURCE}/array_output.cc\
    $${PPAPI_CPP_SOURCE}/audio_buffer.cc\
    $${PPAPI_CPP_SOURCE}/audio_config.cc\
    $${PPAPI_CPP_SOURCE}/audio_encoder.cc\
    $${PPAPI_CPP_SOURCE}/audio.cc\
    $${PPAPI_CPP_SOURCE}/compositor_layer.cc\
    $${PPAPI_CPP_SOURCE}/compositor.cc\
    $${PPAPI_CPP_SOURCE}/core.cc\
    $${PPAPI_CPP_SOURCE}/cursor_control_dev.cc\
    $${PPAPI_CPP_SOURCE}/directory_entry.cc\
    $${PPAPI_CPP_SOURCE}/file_chooser_dev.cc\
    $${PPAPI_CPP_SOURCE}/file_io.cc\
    $${PPAPI_CPP_SOURCE}/file_ref.cc\
    $${PPAPI_CPP_SOURCE}/file_system.cc\
    $${PPAPI_CPP_SOURCE}/font_dev.cc\
    $${PPAPI_CPP_SOURCE}/fullscreen.cc\
    $${PPAPI_CPP_SOURCE}/graphics_2d.cc\
    $${PPAPI_CPP_SOURCE}/graphics_3d_client.cc\
    $${PPAPI_CPP_SOURCE}/graphics_3d.cc\
    $${PPAPI_CPP_SOURCE}/host_resolver.cc\
    $${PPAPI_CPP_SOURCE}/image_data.cc\
    $${PPAPI_CPP_SOURCE}/input_event.cc\
    $${PPAPI_CPP_SOURCE}/instance_handle.cc\
    $${PPAPI_CPP_SOURCE}/instance.cc\
    $${PPAPI_CPP_SOURCE}/lock.cc\
    $${PPAPI_CPP_SOURCE}/media_stream_audio_track.cc\
    $${PPAPI_CPP_SOURCE}/media_stream_video_track.cc\
    $${PPAPI_CPP_SOURCE}/memory_dev.cc\
    $${PPAPI_CPP_SOURCE}/message_loop.cc\
    $${PPAPI_CPP_SOURCE}/module.cc\
    $${PPAPI_CPP_SOURCE}/mouse_cursor.cc\
    $${PPAPI_CPP_SOURCE}/mouse_lock.cc\
    $${PPAPI_CPP_SOURCE}/net_address.cc\
    $${PPAPI_CPP_SOURCE}/network_list.cc\
    $${PPAPI_CPP_SOURCE}/network_monitor.cc\
    $${PPAPI_CPP_SOURCE}/network_proxy.cc\
    $${PPAPI_CPP_SOURCE}/paint_aggregator.cc\
    $${PPAPI_CPP_SOURCE}/paint_manager.cc\
    $${PPAPI_CPP_SOURCE}/ppp_entrypoints.cc\
    $${PPAPI_CPP_SOURCE}/printing_dev.cc\
    $${PPAPI_CPP_SOURCE}/rect.cc\
    $${PPAPI_CPP_SOURCE}/resource.cc\
    $${PPAPI_CPP_SOURCE}/scriptable_object_deprecated.cc\
    $${PPAPI_CPP_SOURCE}/simple_thread.cc\
    $${PPAPI_CPP_SOURCE}/tcp_socket.cc\
    $${PPAPI_CPP_SOURCE}/text_input_controller.cc\
    $${PPAPI_CPP_SOURCE}/truetype_font_dev.cc\
    $${PPAPI_CPP_SOURCE}/udp_socket.cc\
    $${PPAPI_CPP_SOURCE}/url_loader.cc\
    $${PPAPI_CPP_SOURCE}/url_request_info.cc\
    $${PPAPI_CPP_SOURCE}/url_response_info.cc\
    $${PPAPI_CPP_SOURCE}/var_array_buffer.cc\
    $${PPAPI_CPP_SOURCE}/var_array.cc\
    $${PPAPI_CPP_SOURCE}/var_dictionary.cc\
    $${PPAPI_CPP_SOURCE}/var.cc\
    $${PPAPI_CPP_SOURCE}/video_decoder.cc\
    $${PPAPI_CPP_SOURCE}/video_encoder.cc\
    $${PPAPI_CPP_SOURCE}/video_frame.cc\
    $${PPAPI_CPP_SOURCE}/view_dev.cc\
    $${PPAPI_CPP_SOURCE}/view.cc\
    $${PPAPI_CPP_SOURCE}/websocket_api.cc\
    $${PPAPI_CPP_SOURCE}/websocket.cc


# libppapi (stub)
SOURCES += \
    $${PWD}/3rdparty/pepper.js/stub.cc
    # rest of libppapi is implemented in JavasScript and added to the build
    # with '--pre-js' at link time.

# Emscripten provides a better GLES2 API directly than pepper.js
# TODO Consider adding back this when pepper.js has better GLES2 support
# OpenGL
# PPAPI_GLES_SOURCE= $$(NACL_SDK_ROOT)/src/ppapi_gles2
# SOURCES +=\
#    $${PPAPI_GLES_SOURCE}/gles2.c \
#    $${PPAPI_GLES_SOURCE}/gl2ext_ppapi.c \
#}

OTHER_FILES += \
    $$PWD/3rdparty/pepper.js/audio.js \
    $$PWD/3rdparty/pepper.js/base.js \
    $$PWD/3rdparty/pepper.js/common.js \
    $$PWD/3rdparty/pepper.js/file.js \
    $$PWD/3rdparty/pepper.js/gles.js \
    $$PWD/3rdparty/pepper.js/gles_ext.js \
    $$PWD/3rdparty/pepper.js/graphics_2d.js \
    $$PWD/3rdparty/pepper.js/graphics_3d.js \
    $$PWD/3rdparty/pepper.js/input_events.js \
    $$PWD/3rdparty/pepper.js/load_deps.js \
    $$PWD/3rdparty/pepper.js/loadnacl.js \
    $$PWD/3rdparty/pepper.js/mouse_lock.js \
    $$PWD/3rdparty/pepper.js/ppapi_preamble.js \
    $$PWD/3rdparty/pepper.js/testing.js \
    $$PWD/3rdparty/pepper.js/url_loader.js \
    $$PWD/3rdparty/pepper.js/view.js \
    $$PWD/3rdparty/pepper.js/web_socket.js \
    $$PWD/3rdparty/pepper.js/LICENSE
