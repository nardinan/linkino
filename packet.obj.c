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
#include "packet.obj.h"
struct s_packet_attributes *p_packet_alloc(struct s_object *self) {
  struct s_packet_attributes *result = d_prepare(self, packet);
  f_memory_new(self);                                                                 /* inherit */
  f_mutex_new(self);                                                                  /* inherit */
  f_drawable_new(self, (e_drawable_kind_single | e_drawable_kind_force_visibility));  /* inherit */
  f_eventable_new(self);                                                              /* inherit */
  return result;
}
struct s_object *f_packet_new(struct s_object *self, struct s_object *ui_factory, struct s_object *drawable_icon, struct s_object *drawable_background, 
    const char *body_content, int flags) {
  struct s_packet_attributes *packet_attributes = p_packet_alloc(self);
  packet_attributes->drawable_icon = d_retain(drawable_icon);
  packet_attributes->drawable_background = d_retain(drawable_background);
  d_assert(packet_attributes->ui_label_content = d_call(ui_factory, m_ui_factory_new_label, d_ui_factory_default_font_id, d_ui_factory_default_font_style,
        body_content));
  d_assert(packet_attributes->ui_button_close = d_call(ui_factory, m_ui_factory_new_button, d_ui_factory_default_font_id, d_ui_factory_default_font_style,
        "please, close"));
  packet_attributes->traveling_speed = 1.0;
  packet_attributes->flags = flags;
  return self;
}
d_define_method(packet, set_traveling)(struct s_object *self, t_boolean traveling, struct s_object *connector_traveling, double current_position, 
    double destination_position) {
  d_using(packet);
  if ((packet_attributes->traveling = traveling)) {
    packet_attributes->connector_traveling = d_retain(connector_traveling);
    packet_attributes->current_position = current_position;
    packet_attributes->destination_position = destination_position;
  }
  return self;
}
d_define_method(packet, set_analyzing)(struct s_object *self, t_boolean analyzing) {
  d_using(packet);
  packet_attributes->analyzing = analyzing;
  return self;
}
d_define_method(packet, set_traveling_speed)(struct s_object *self, double traveling_speed) {
  d_using(packet);
  packet_attributes->traveling_speed = traveling_speed;
  return self;
}
d_define_method(packet, set_direction)(struct s_object *self, double direction) {
  d_using(packet);
  packet_attributes->current_direction = direction;
  return self;
}
d_define_method(packet, is_arrived)(struct s_object *self) {
  d_using(packet);
  d_cast_return((packet_attributes->current_position == packet_attributes->destination_position));
}
d_define_method(packet, move_by)(struct s_object *self, double movement) {
  d_using(packet);
  /* if the packet is not traveling, the movement is ignored, obviously */
  if (packet_attributes->traveling) {
    /* the movement has to be normalized between zero and one, the direction is given by the system directly */
    packet_attributes->current_position += ((movement * packet_attributes->current_direction) * packet_attributes->traveling_speed);
    if (packet_attributes->current_position > 1.0)
      packet_attributes->current_position = 1.0;
    else if (packet_attributes->current_position < 0.0)
      packet_attributes->current_position = 0.0;
  }
  return self;
}
d_define_method_override(packet, event)(struct s_object *self, struct s_object *environment, SDL_Event *current_event) {
  t_boolean changed = d_false;
  d_cast_return(((changed) ? e_eventable_status_captured : e_eventable_status_ignored));
}
d_define_method_override(packet, draw)(struct s_object *self, struct s_object *environment) {
  d_using(packet);
  struct s_environment_attributes *environment_attributes = d_cast(environment, environment);
  struct s_camera_attributes *camera_attributes = d_cast(environment_attributes->current_camera, camera);
  if (packet_attributes->traveling) {
    double position_x, position_y;
    /* OK, we are traveling and this means that the packet icon is currently visible around the screen. This means that, since the current position
     * of the packet is stored into the 'current_position' attribute, we should check if we have a connector associated and take from it the
     * real 2D position of the packet.
     */
    if ((packet_attributes->connector_traveling) && 
        (d_call(packet_attributes->connector_traveling, m_connector_get_point, packet_attributes->current_position, &position_x, &position_y))) {
      double icon_width, icon_height;
      d_call(packet_attributes->drawable_icon, m_drawable_get_dimension, &icon_width, &icon_height);
      d_call(packet_attributes->drawable_icon, m_drawable_set_position, (double)(position_x - (icon_width / 2.0)), 
          (double)(position_y - (icon_height / 2.0)));
      if ((d_call(packet_attributes->drawable_icon, m_drawable_normalize_scale, camera_attributes->scene_reference_w, camera_attributes->scene_reference_h,
        camera_attributes->scene_offset_x, camera_attributes->scene_offset_y, camera_attributes->scene_center_x, camera_attributes->scene_center_y,
        camera_attributes->screen_w, camera_attributes->screen_h, camera_attributes->scene_zoom)))
        while (((intptr_t)d_call(packet_attributes->drawable_icon, m_drawable_draw, environment)) == d_drawable_return_continue);
    }
  }
  d_cast_return(d_drawable_return_last);
}
d_define_method(packet, delete)(struct s_object *self, struct s_packet_attributes *attributes) {
  d_delete(attributes->drawable_icon);
  d_delete(attributes->drawable_background);
  d_delete(attributes->ui_label_content);
  d_delete(attributes->ui_button_close);
  return NULL;
}
d_define_class(packet) {d_hook_method(packet, e_flag_public, set_traveling),
                        d_hook_method(packet, e_flag_public, set_analyzing),
                        d_hook_method(packet, e_flag_public, set_traveling_speed),
                        d_hook_method(packet, e_flag_public, set_direction),
                        d_hook_method(packet, e_flag_public, is_arrived),
                        d_hook_method(packet, e_flag_public, move_by),
                        d_hook_method_override(packet, e_flag_public, eventable, event),
                        d_hook_method_override(packet, e_flag_public, drawable, draw),
                        d_hook_delete(packet),
                        d_hook_method_tail};
