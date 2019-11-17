/*
 * linkino
 * Copyright (C) 2020 Andrea Nardinocchi (andrea@nardinan.it)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "loader.obj.h"
#include "director.obj.h"
struct s_object *loader;
struct s_object *label_camera;
struct s_object *default_draw_camera, *default_ui_camera;
t_boolean v_initialized;
int linkino_load_call_start(struct s_object *environment) {
  if ((loader = f_loader_new(d_new(loader), environment))) {
    d_call(loader, m_runnable_run, NULL);
  }
  return d_true;
}
int linkino_loop_call(struct s_object *environment) {
  if (!v_initialized) {
    if (!d_call(loader, m_runnable_running, NULL)) {
      struct s_loader_attributes *loader_attributes = d_cast(loader, loader);
      struct s_media_factory_attributes *media_factory_attributes = d_cast(loader_attributes->media_factory, media_factory);
      struct s_object *director = f_director_new(d_new(director), environment, loader_attributes->ui_factory, loader_attributes->media_factory);
      double offsets_x_router[] = {
        0.0,
        0.0,
        0.0,
        40.0,
        40.0
      }, offsets_y_router[] = {
        5.0,
        20.0,
        33.0,
        20.0,
        33.0
      }, offsets_x_firewall[] = {
        0.0,
        42.0
      }, offsets_y_firewall[] = {
        22.0,
        22.0
      }, offsets_x_computer[] = {
        84.0
      }, offsets_y_computer[] = {
        59.0
      };
      d_call(director, m_director_add_node, "router", "ROUTER", "A router that provides the basic infrastructure between nodes", offsets_x_router,
        offsets_y_router, (size_t)5, d_false, d_false);
      d_call(director, m_director_add_node, "firewall", "FIREWALL", "A firewall that protect the peer 2 peer communication channels", offsets_x_firewall,
        offsets_y_firewall, (size_t)2, d_false, d_true);
      d_call(director, m_director_add_node, "computer", "PC STATION", "A basic PC required by every employee in the company", offsets_x_computer,
        offsets_y_computer, (size_t)1, d_true, d_false);
      d_call(environment, m_environment_add_drawable, director, (d_ui_factory_default_level), e_environment_surface_primary);
      v_initialized = d_true;
    }
  }
  return d_true;
}
int linkino_quit_call(struct s_object *environment) {
  /* we need to wait for the end of the loading procedure */
  if (!v_initialized)
    d_call(loader, m_runnable_join, NULL);
  d_delete(loader);
  return d_true;
}
void linkino_change_location(const char *application) {
  char buffer[PATH_MAX], *pointer;
  if ((pointer = strrchr(application, '/'))) {
    memset(buffer, 0, PATH_MAX);
    strncpy(buffer, application, (pointer - application));
    chdir(buffer);
    d_log(e_log_level_ever, "changing local path to: %s", buffer);
  }
}
int main(int argc, char *argv[]) {
  struct s_exception *exception;
  struct s_object *environment;
  struct s_object *stream_configuration;
  struct s_object *json_configuration;
  double scale_resolution_x, scale_resolution_y;
  struct s_environment_attributes *environment_attributes;
  d_pool_init;
  d_pool_begin("main context")
  {
    v_log_level = e_log_level_ever;
    /* change chmod to the current location of the application */
    linkino_change_location(argv[0]);
    label_camera = d_pkstr("draw_camera");
    d_try
    {
      if ((stream_configuration = f_stream_new_file(d_new(stream), d_pkstr(d_linkino_resources_configuration), "r", 0777))) {
        if ((json_configuration = f_json_new_stream(d_new(json), stream_configuration))) {
          d_call(json_configuration, m_json_get_double, &v_linkino_width_window, "ss", "window", "width");
          d_call(json_configuration, m_json_get_double, &v_linkino_height_window, "ss", "window", "height");
          d_call(json_configuration, m_json_get_boolean, &v_linkino_fullscreen, "ss", "window", "fullscreen");
          d_call(json_configuration, m_json_get_double, &v_linkino_width_reference, "ss", "geometry", "width");
          d_call(json_configuration, m_json_get_double, &v_linkino_height_reference, "ss", "geometry", "height");
          d_call(json_configuration, m_json_get_double, &v_linkino_scale_factor, "ss", "geometry", "scale");
          d_call(json_configuration, m_json_get_double, &v_linkino_effects_volume, "s", "volume_effects");
          d_call(json_configuration, m_json_get_double, &v_linkino_music_volume, "s", "volume_music");
          d_delete(json_configuration);
        }
        d_delete(stream_configuration);
      }
      scale_resolution_x = (v_linkino_width_reference * v_linkino_scale_factor);
      scale_resolution_y = (v_linkino_height_reference * v_linkino_scale_factor);
      environment = f_environment_new_fullscreen(d_new(environment), v_linkino_width_window, v_linkino_height_window, v_linkino_fullscreen);
      d_call(environment, m_environment_set_methods, &linkino_load_call_start, &linkino_loop_call, &linkino_quit_call);
      environment_attributes = d_cast(environment, environment);
      if ((default_draw_camera =
             f_camera_new(d_new(camera), 0.0, 0.0, environment_attributes->current_w, environment_attributes->current_h, e_environment_surface_primary,
               environment))) {
        d_call(default_draw_camera, m_camera_set_reference, (double)scale_resolution_x, (double)scale_resolution_y);
        d_call(default_draw_camera, m_camera_set_center, (double)0.0, (double)0.0);
        if ((default_ui_camera =
               f_camera_new(d_new(camera), 0.0, 0.0, environment_attributes->current_w, environment_attributes->current_h, e_environment_surface_ui, environment))) {
          d_call(default_ui_camera, m_camera_set_reference, (double)scale_resolution_x, (double)scale_resolution_y);
          d_call(default_ui_camera, m_camera_set_center, (double)0.0, (double)0.0);
          d_call(environment, m_environment_add_camera, label_camera, default_draw_camera);
          d_call(environment, m_environment_add_camera, d_pkstr("ui_camera"), default_ui_camera);
          d_call(environment, m_environment_set_title, d_linkino_title);
          d_call(environment, m_environment_set_channels, d_media_factory_max_channels);
          d_call(environment, m_environment_run_loop, NULL);
        }
      }
      d_delete(environment);
    }
    d_catch(exception)
    {
      d_exception_dump(stderr, exception);
    }
    d_endtry;
  }
  d_pool_end;
  d_pool_destroy;
  f_memory_destroy();
  return 0;
}
