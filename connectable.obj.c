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
struct s_connectable_attributes *p_connectable_alloc(struct s_object *self) {
  struct s_connectable_attributes *result = d_prepare(self, connectable);
  /* abstract (no memory inheritance) */
  f_drawable_new(self, (e_drawable_kind_single | e_drawable_kind_force_visibility));  /* inherit */
  f_eventable_new(self);                                                              /* inherit */
  return result;
}
extern struct s_object *f_connectable_new(struct s_object *self) {
  struct s_connectable_attributes *connectable_attributes = p_connectable_alloc(self);
  memset(&(connectable_attributes->list_connection_nodes), 0, sizeof(struct s_list));
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
    strncpy(connectable_link->label, label, d_string_buffer_size);
    f_list_append(&(connectable_attributes->list_connection_nodes), (struct s_list_node *)connectable_link, e_list_insert_tail);
  } else
    d_die(d_error_malloc);
  return self;
}
d_define_method(connectable, get_selected_node)(struct s_object *self) {
  d_using(connectable);
  struct s_connection_node *result = connectable_attributes->selected_node;
  /* the routine takes the latest selected node, then it resets the pointer to NULL in order to be ready for the next release */
  connectable_attributes->selected_node = NULL;
  d_cast_return(result);
}
d_define_method_override(connectable, event)(struct s_object *self, struct s_object *environment, SDL_Event *current_event) {
  d_using(connectable);
  t_boolean changed = d_false;
  int mouse_x, mouse_y;
  SDL_GetMouseState(&mouse_x, &mouse_y);
  if (current_event->type == SDL_MOUSEBUTTONDOWN) {
    if (current_event->button.button == SDL_BUTTON_LEFT) {
      struct s_connection_node *connection_node;
      double position_x, position_y;
      d_call(self, m_drawable_get_position, &position_x, &position_y);
      d_foreach(&(connectable_attributes->list_connection_nodes), connection_node, struct s_connection_node) {
        if ((mouse_x >= (position_x + connection_node->offset_x)) && (mouse_x <= (position_x + connection_node->offset_x + connection_node->width)) &&
            (mouse_y >= (position_y + connection_node->offset_y)) && (mouse_y <= (position_y + connection_node->offset_y + connection_node->height))) {
          connectable_attributes->selected_node = connection_node;
          changed = d_true;
          break;
        }
      }
    }
  }
  d_cast_return(((changed) ? e_eventable_status_captured : e_eventable_status_ignored));
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
                             d_hook_delete(connectable),
                             d_hook_method_tail};