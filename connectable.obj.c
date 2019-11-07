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
#include "connectable.obj.h"
struct s_connectable_attributes *p_connectable_alloc(struct s_object *self, struct s_object *stream, struct s_object *environment) {
  struct s_connectable_attributes *result = d_prepare(self, connectable);
  f_bitmap_new(self, stream, environment);  /* inherit */
  f_eventable_new(self);                    /* inherit */
  return result;
}
extern struct s_object *f_connectable_new(struct s_object *self, struct s_object *stream, struct s_object *environment, const char *unique_code) {
  struct s_connectable_attributes *connectable_attributes = p_connectable_alloc(self, stream, environment);
  memset(&(connectable_attributes->list_connection_nodes), 0, sizeof(struct s_list));
  strncpy(connectable_attributes->unique_code, unique_code, d_connectable_code_size);
  return self;
}
d_define_method(connectable, add_connection_point)(struct s_object *self, double offset_x, double offset_y, const char *label) {
  d_using(connectable);
  struct s_connection_node *connectable_link;
  if ((connectable_link = (struct s_connection_node *) d_malloc(sizeof(struct s_connection_node)))) {
    connectable_link->offset_x = offset_x;
    connectable_link->offset_y = offset_y;
    connectable_link->width = d_connectable_width;
    connectable_link->height = d_connectable_height;
    connectable_link->connectable = self; /* a reentrant point to access the object through the link */
    strncpy(connectable_link->label, label, d_string_buffer_size);
    strncpy(connectable_link->unique_code, connectable_attributes->unique_code, d_connectable_code_size);
    f_list_append(&(connectable_attributes->list_connection_nodes), (struct s_list_node *)connectable_link, e_list_insert_tail);
  } else
    d_die(d_error_malloc);
  return self;
}
d_define_method(connectable, get_selected_node)(struct s_object *self) {
  d_using(connectable);
  struct s_connection_node *result = connectable_attributes->active_node;
  /* the routine takes the latest selected node, then it resets the pointer to NULL in order to be ready for the next release */
  connectable_attributes->active_node = NULL;
  d_cast_return(result);
}
d_define_method_override(connectable, event)(struct s_object *self, struct s_object *environment, SDL_Event *current_event) {
  d_using(connectable);
  struct s_connection_node *connection_node;
  struct s_drawable_attributes *drawable_attributes = d_cast(self, drawable);
  t_boolean changed = d_false;
  double position_x, position_y;
  int mouse_x, mouse_y;
  SDL_GetMouseState(&mouse_x, &mouse_y);
  d_call(&(drawable_attributes->point_destination), m_point_get, &position_x, &position_y);
  connectable_attributes->draw_rectangle = d_false;
  d_foreach(&(connectable_attributes->list_connection_nodes), connection_node, struct s_connection_node) {
    /* update the position of the nodes in respect of the final position of the connectable */
    connection_node->final_position_x = (connection_node->offset_x + position_x + (connection_node->width / 2.0));
    connection_node->final_position_y = (connection_node->offset_y + position_y + (connection_node->height / 2.0));
    if ((mouse_x >= (position_x + connection_node->offset_x)) && (mouse_x <= (position_x + connection_node->offset_x + connection_node->width)) &&
        (mouse_y >= (position_y + connection_node->offset_y)) && (mouse_y <= (position_y + connection_node->offset_y + connection_node->height))) {
      if ((current_event->type == SDL_MOUSEBUTTONDOWN) && (current_event->button.button == SDL_BUTTON_LEFT)) {
        connectable_attributes->active_node = connection_node;
      }
      connectable_attributes->rectangle_x[0] = connection_node->offset_x + position_x;
      connectable_attributes->rectangle_y[0] = connection_node->offset_y + position_y;
      connectable_attributes->rectangle_x[1] = connection_node->offset_x + position_x;
      connectable_attributes->rectangle_y[1] = connection_node->offset_y + position_y + connection_node->height;
      connectable_attributes->rectangle_x[2] = connection_node->offset_x + position_x + connection_node->width;
      connectable_attributes->rectangle_y[2] = connection_node->offset_y + position_y + connection_node->height;
      connectable_attributes->rectangle_x[3] = connection_node->offset_x + position_x + connection_node->width;
      connectable_attributes->rectangle_y[3] = connection_node->offset_y + position_y;
      connectable_attributes->draw_rectangle = d_true;
      changed = d_true;
    }
  }
  d_cast_return(((changed) ? e_eventable_status_captured : e_eventable_status_ignored));
}
d_define_method_override(connectable, draw)(struct s_object *self, struct s_object *environment) {
  d_using(connectable);
  struct s_environment_attributes *environment_attributes = d_cast(environment, environment);
  int result = (intptr_t)d_call_owner(self, bitmap, m_drawable_draw, environment); /* recall the father's draw method */
  if (connectable_attributes->draw_rectangle) {
    f_primitive_fill_polygon(environment_attributes->renderer, connectable_attributes->rectangle_x, connectable_attributes->rectangle_y,
      d_connectable_rectangle_elements, d_connectable_rectangle_R, d_connectable_rectangle_G, d_connectable_rectangle_B, d_connectable_rectangle_A);
  }
  d_cast_return(result);
}
d_define_method(connectable, delete)(struct s_object *self, struct s_connectable_attributes *attributes) {
  struct s_connection_node *current_element;
  while ((current_element = (struct s_connection_node *)attributes->list_connection_nodes.head)) {
    f_list_delete(&(attributes->list_connection_nodes), (struct s_list_node *)current_element);
    d_free(current_element);
  }
  return NULL;
}
d_define_class(connectable) {d_hook_method(connectable, e_flag_public, add_connection_point),
                             d_hook_method(connectable, e_flag_public, get_selected_node),
                             d_hook_method_override(connectable, e_flag_public, eventable, event),
                             d_hook_method_override(connectable, e_flag_public, drawable, draw),
                             d_hook_delete(connectable),
                             d_hook_method_tail};