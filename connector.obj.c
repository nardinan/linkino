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
#include "connector.obj.h"
struct s_connector_attributes *p_connector_alloc(struct s_object *self) {
  struct s_connector_attributes *result = d_prepare(self, connector);
  f_memory_new(self);                                                                 /* inherit */
  f_mutex_new(self);                                                                  /* inherit */
  f_drawable_new(self, (e_drawable_kind_single | e_drawable_kind_force_visibility));  /* inherit */
  return result;
}
struct s_object *f_connector_new(struct s_object *self, struct s_object *drawable, double source_x, double source_y, struct s_connection_node *link) {
  struct s_connector_attributes *connector_attributes = p_connector_alloc(self);
  if ((connector_attributes->drawable = d_retain(drawable))) {
    connector_attributes->separation = 0.5; /* by default is half of the line */
    d_assert(connector_attributes->starting_point = f_point_new(d_new(point), source_x, source_y));
  }
  connector_attributes->source_link = link;
  return self;
}
d_define_method(connector, set_starting)(struct s_object *self, double starting_x, double starting_y, struct s_connection_node *link) {
  d_using(connector);
  if (connector_attributes->drawable)
    d_call(connector_attributes->starting_point, m_point_set, starting_x, starting_y);
  connector_attributes->source_link = link;
  return self;
}
d_define_method(connector, set_destination)(struct s_object *self, double destination_x, double destination_y, struct s_connection_node *link) {
  d_using(connector);
  if (connector_attributes->drawable) {
    if (connector_attributes->destination_point)
      d_call(connector_attributes->destination_point, m_point_set, destination_x, destination_y);
    else
      d_assert(connector_attributes->destination_point = f_point_new(d_new(point), destination_x, destination_y));
  }
  connector_attributes->destination_link = link;
  return self;
}
d_define_method_override(connector, draw)(struct s_object *self, struct s_object *environment) {
  d_using(connector);
  struct s_environment_attributes *environment_attributes = d_cast(environment, environment);
  struct s_camera_attributes *camera_attributes = d_cast(environment_attributes->current_camera, camera);
  if ((connector_attributes->destination_point) && (connector_attributes->drawable)) {
    double starting_position_x, starting_position_y, final_position_x, final_position_y, length_first_half = 0, length_second_half = 0, drawable_width,
      drawable_height, higher_vertical_position, lower_vertical_position;
    t_boolean reached;
    d_call(connector_attributes->drawable, m_drawable_get_dimension, &drawable_width, &drawable_height);
    d_call(connector_attributes->starting_point, m_point_get, &starting_position_x, &starting_position_y);
    d_call(connector_attributes->destination_point, m_point_get, &final_position_x, &final_position_y);
    /* We consider the starting point the leftmost point, and the final one the other one. We also have to consider the highest
     * and the lowest vertical position.
     */
    if (starting_position_x > final_position_x) {
      double backup_position_x = starting_position_x, backup_position_y = starting_position_y;
      starting_position_x = final_position_x;
      starting_position_y = final_position_y;
      final_position_x = backup_position_x;
      final_position_y = backup_position_y;
    }
    if (final_position_y > starting_position_y) {
      higher_vertical_position = final_position_y;
      lower_vertical_position = starting_position_y;
    } else {
      higher_vertical_position = starting_position_y;
      lower_vertical_position = final_position_y;
    }
    /* The y component has to be shifted because the pivoting of the object is done in respect of its center. This means that the
     * whole line will move down by half of the width of the image */
    higher_vertical_position += (drawable_width / 2.0);
    lower_vertical_position += (drawable_width / 2.0);
    /* The line is composed by two (or three) components that are associated respectively to a horizontal displacement,
     * a vertical displacement and another horizontal displacement:
     *  - first horizontal part (displacement in X)
     *  - vertical optional part (displacement in Y)
     *  - second horizontal part (displacement in X)
     *  The separation between the first and the second line happens at 0.5 (or, more precisely at "separation") of the total horizontal
     *  length of the line.
     */
    length_first_half = connector_attributes->separation * (final_position_x - starting_position_x) - drawable_height;
    length_second_half = (1.0 - connector_attributes->separation) * (final_position_x - starting_position_x) - drawable_height;
    d_call(connector_attributes->drawable, m_drawable_set_angle, 0.0);
    reached = d_false;
    for (double position_x = starting_position_x; (!reached); position_x += drawable_width) {
      d_call(connector_attributes->drawable, m_drawable_set_position, position_x, starting_position_y);
      if (position_x >= (starting_position_x + length_first_half)) {
        break;
      } else if ((position_x + drawable_width) > (starting_position_x + length_first_half)) {
        d_call(connector_attributes->drawable, m_drawable_set_dimension_w, ((starting_position_x + length_first_half) - position_x));
        reached = d_true;
      } else
        d_call(connector_attributes->drawable, m_drawable_set_dimension_w, drawable_width);
      if ((d_call(connector_attributes->drawable, m_drawable_normalize_scale, camera_attributes->scene_reference_w, camera_attributes->scene_reference_h,
        camera_attributes->scene_offset_x, camera_attributes->scene_offset_y, camera_attributes->scene_center_x, camera_attributes->scene_center_y,
        camera_attributes->screen_w, camera_attributes->screen_h, camera_attributes->scene_zoom)))
        while (((intptr_t)d_call(connector_attributes->drawable, m_drawable_draw, environment)) == d_drawable_return_continue);
    }
    reached = d_false;
    for (double position_x = (final_position_x - length_second_half); (!reached); position_x += drawable_width) {
      d_call(connector_attributes->drawable, m_drawable_set_position, position_x, final_position_y);
      if (position_x >= final_position_x) {
        break;
      } else if ((position_x + drawable_width) > final_position_x) {
        d_call(connector_attributes->drawable, m_drawable_set_dimension_w, (final_position_x - position_x));
        reached = d_true;
      } else
        d_call(connector_attributes->drawable, m_drawable_set_dimension_w, drawable_width);
      if ((d_call(connector_attributes->drawable, m_drawable_normalize_scale, camera_attributes->scene_reference_w, camera_attributes->scene_reference_h,
        camera_attributes->scene_offset_x, camera_attributes->scene_offset_y, camera_attributes->scene_center_x, camera_attributes->scene_center_y,
        camera_attributes->screen_w, camera_attributes->screen_h, camera_attributes->scene_zoom)))
        while (((intptr_t)d_call(connector_attributes->drawable, m_drawable_draw, environment)) == d_drawable_return_continue);
    }
    if (higher_vertical_position > lower_vertical_position) {
      d_call(connector_attributes->drawable, m_drawable_set_angle, 90.0);
      reached = d_false;
      for (double position_y = lower_vertical_position; (!reached); position_y += drawable_width) {
        d_call(connector_attributes->drawable, m_drawable_set_position, (starting_position_x + length_first_half), position_y);
        if (position_y >= higher_vertical_position) {
          break;
        } else if ((position_y + drawable_width) > higher_vertical_position) {
          d_call(connector_attributes->drawable, m_drawable_set_dimension_w, (higher_vertical_position - position_y));
          reached = d_true;
        } else
          d_call(connector_attributes->drawable, m_drawable_set_dimension_w, drawable_width);
        if ((d_call(connector_attributes->drawable, m_drawable_normalize_scale, camera_attributes->scene_reference_w, camera_attributes->scene_reference_h,
          camera_attributes->scene_offset_x, camera_attributes->scene_offset_y, camera_attributes->scene_center_x, camera_attributes->scene_center_y,
          camera_attributes->screen_w, camera_attributes->screen_h, camera_attributes->scene_zoom)))
          while (((intptr_t)d_call(connector_attributes->drawable, m_drawable_draw, environment)) == d_drawable_return_continue);
      }
    }
    /* we need to restore the right size before leaving */
    d_call(connector_attributes->drawable, m_drawable_set_dimension_w, drawable_width);
  }
  d_cast_return(d_drawable_return_last);
}
d_define_method(connector, delete)(struct s_object *self, struct s_connector_attributes *attributes) {
  if (attributes->drawable)
    d_delete(attributes->drawable);
  if (attributes->starting_point)
    d_delete(attributes->starting_point);
  if (attributes->destination_point)
    d_delete(attributes->destination_point);
  return NULL;
}
d_define_class(connector) {d_hook_method(connector, e_flag_public, set_starting),
                           d_hook_method(connector, e_flag_public, set_destination),
                           d_hook_method_override(connector, e_flag_public, drawable, draw),
                           d_hook_delete(connector),
                           d_hook_method_tail};