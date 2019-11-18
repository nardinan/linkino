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
unsigned int index_human_name = 0;
const char *list_human_names[] = {
  "David",
  "John",
  "Paul",
  "Susan",
  "Claire",
  "Sharon",
  "Angela",
  "Gillian",
  "Julie",
  "Michelle",
  "William",
  "Craig",
  "Michael",
  "Louise",
  "Alison",
  "Colin",
  "Donna",
  "Caroline",
  "Gary",
  "Lynn",
  "Derek",
  "Martin",
  "Lesley",
  "Deborah",
  "Pauline",
  "Lorraine",
  "Laura",
  "Iain",
  "Gordon",
  "Alexander",
  "Linda",
  "Peter",
  "Catherine",
  "Wendy",
  "Lynne",
  "Kenneth",
  "Pamela",
  "Kirsty",
  "Douglas",
  "Emma",
  "Joanne",
  "Heather",
  "Grant",
  "Anne",
  "Diane",
  "Gavin",
  NULL
};
struct s_connectable_attributes *p_connectable_alloc(struct s_object *self, struct s_object *stream, struct s_object *environment) {
  struct s_connectable_attributes *result = d_prepare(self, connectable);
  f_bitmap_new(self, stream, environment);  /* inherit */
  f_eventable_new(self);                    /* inherit */
  return result;
}
extern struct s_object *f_connectable_new(struct s_object *self, struct s_object *stream, struct s_object *environment, struct s_object *ui_factory, 
    t_boolean use_human_name, t_boolean block_spam) {
  struct s_connectable_attributes *connectable_attributes = p_connectable_alloc(self, stream, environment);
  memset(&(connectable_attributes->list_connection_nodes), 0, sizeof(struct s_list));
  if (use_human_name) {
    strncpy(connectable_attributes->unique_code, list_human_names[index_human_name++], d_connectable_code_size);
    d_assert((connectable_attributes->ui_label = d_call(ui_factory, m_ui_factory_new_label, d_ui_factory_default_font_id, 
            d_ui_factory_default_font_style, connectable_attributes->unique_code)));
  } else {
    for (unsigned int index = 0; index < (d_connectable_code_size - 1); ++index)
      connectable_attributes->unique_code[index] = ((rand() % (((char)'Z') - ((char)'A'))) + (char)'A');
  }
  connectable_attributes->seconds_between_generation_minimum = d_connectable_min_seconds_between_generation;
  connectable_attributes->seconds_between_generation_maximum = d_connectable_max_seconds_between_generation;
  connectable_attributes->block_spam = block_spam;
  return self;
}
d_define_method(connectable, set_generate_traffic)(struct s_object *self, t_boolean generate_traffic) {
  d_using(connectable);
  connectable_attributes->generate_traffic = generate_traffic;
  return self;
}
d_define_method(connectable, set_generate_traffic_speed)(struct s_object *self, time_t minimum_seconds_between_traffic,
    time_t maximum_seconds_between_traffic) {
  d_using(connectable);
  connectable_attributes->seconds_between_generation_minimum = minimum_seconds_between_traffic;
  connectable_attributes->seconds_between_generation_maximum = maximum_seconds_between_traffic;
  return self;
}
d_define_method(connectable, set_price)(struct s_object *self, double price) {
  d_using(connectable);
  connectable_attributes->price = price;
  return self;
}
d_define_method(connectable, add_connection_point)(struct s_object *self, double offset_x, double offset_y, const char *label) {
  d_using(connectable);
  struct s_connectable_link *connectable_link;
  if ((connectable_link = (struct s_connectable_link *) d_malloc(sizeof(struct s_connectable_link)))) {
    connectable_link->offset_x = offset_x;
    connectable_link->offset_y = offset_y;
    connectable_link->width = d_connectable_width;
    connectable_link->height = d_connectable_height;
    connectable_link->connectable = self; /* a reentrant point to access the object through the link */
    strncpy(connectable_link->label, label, d_string_buffer_size);
    strncpy(connectable_link->unique_code, connectable_attributes->unique_code, d_connectable_code_size);
    memset(connectable_link->traveling_packets, 0, sizeof(struct s_object *) * d_connectable_max_packets);
    f_list_append(&(connectable_attributes->list_connection_nodes), (struct s_list_node *)connectable_link, e_list_insert_tail);
  } else
    d_die(d_error_malloc);
  return self;
}
d_define_method(connectable, get_selected_node)(struct s_object *self) {
  d_using(connectable);
  struct s_connectable_link *result = connectable_attributes->active_node;
  /* the routine takes the latest selected node, then it resets the pointer to NULL in order to be ready for the next release */
  connectable_attributes->active_node = NULL;
  d_cast_return(result);
}
d_define_method(connectable, is_traffic_generation_required)(struct s_object *self) {
  d_using(connectable);
  struct s_connectable_link *result = NULL;
  if (connectable_attributes->generate_traffic) {
    time_t current_timestamp = time(NULL);
    if (current_timestamp >= connectable_attributes->next_token_generation) {
      size_t final_link_steps = (rand() % (size_t)connectable_attributes->list_connection_nodes.fill);
      result = (struct s_connectable_link *)connectable_attributes->list_connection_nodes.head;
      while ((((struct s_list_node *)result)->next) && (final_link_steps > 0)) {
        result = (struct s_connectable_link *)(((struct s_list_node *)result)->next);
        --final_link_steps;
      }
      /* we need to select, randomly, one of the connectables of the node */
      connectable_attributes->next_token_generation = current_timestamp + 
        (rand() % (connectable_attributes->seconds_between_generation_maximum - connectable_attributes->seconds_between_generation_minimum)) +
        connectable_attributes->seconds_between_generation_minimum;
    }
  }
  d_cast_return(result);
}
d_define_method(connectable, is_over)(struct s_object *self, int mouse_x, int mouse_y) {
  d_using(connectable);
  struct s_drawable_attributes *drawable_attributes = d_cast(self, drawable);
  double position_x, position_y, width, height;
  struct s_object *result = NULL;
  d_call(self, m_drawable_get_position, &position_x, &position_y);
  d_call(self, m_drawable_get_dimension, &width, &height);
  if (((mouse_x > position_x) && (mouse_x < position_x + width)) && 
      ((mouse_y > position_y) && (mouse_y < position_y + height)))
    result = self;
  return result;
}
d_define_method_override(connectable, event)(struct s_object *self, struct s_object *environment, SDL_Event *current_event) {
  d_using(connectable);
  struct s_connectable_link *connection_node;
  struct s_drawable_attributes *drawable_attributes = d_cast(self, drawable);
  double position_x, position_y;
  int mouse_x, mouse_y;
  t_boolean changed = d_false;
  d_call(environment, m_environment_get_mouse_normalized, "draw_camera", &mouse_x, &mouse_y);
  d_call(&(drawable_attributes->point_destination), m_point_get, &position_x, &position_y);
  connectable_attributes->draw_rectangle = d_false;
  d_foreach(&(connectable_attributes->list_connection_nodes), connection_node, struct s_connectable_link) {
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
      connectable_attributes->normalized = d_false;
      connectable_attributes->draw_rectangle = d_true;
      changed = d_true;
    }
  }
  d_cast_return(((changed) ? e_eventable_status_captured : e_eventable_status_ignored));
}
d_define_method_override(connectable, draw)(struct s_object *self, struct s_object *environment) {
  d_using(connectable);
  struct s_environment_attributes *environment_attributes = d_cast(environment, environment);
  struct s_camera_attributes *camera_attributes = d_cast(environment_attributes->current_camera, camera);
  struct s_drawable_attributes *drawable_attributes = d_cast(self, drawable);
  struct s_connectable_link *connection_node;
  double position_x, position_y, width, height;
  int result = (intptr_t)d_call_owner(self, bitmap, m_drawable_draw, environment); /* recall the father's draw method */
  d_call(&(drawable_attributes->point_destination), m_point_get, &position_x, &position_y);
  d_call(&(drawable_attributes->point_dimension), m_point_get, &width, &height);
  if (connectable_attributes->ui_label) {
    double width_label, height_label;
    d_call(connectable_attributes->ui_label, m_drawable_get_dimension, &width_label, &height_label);
    d_call(connectable_attributes->ui_label, m_drawable_set_position, ((position_x + (width / 2.0)) - (width_label / 2.0)), 
        (position_y - height_label));
    if ((d_call(connectable_attributes->ui_label, m_drawable_normalize_scale, camera_attributes->scene_reference_w, camera_attributes->scene_reference_h,
            camera_attributes->scene_offset_x, camera_attributes->scene_offset_y, camera_attributes->scene_center_x, camera_attributes->scene_center_y,
            camera_attributes->screen_w, camera_attributes->screen_h, camera_attributes->scene_zoom)))
      while (((intptr_t)d_call(connectable_attributes->ui_label, m_drawable_draw, environment)) == d_drawable_return_continue);
  }
  /* we need to update the position of the links of the node in respect of its position */
  d_foreach(&(connectable_attributes->list_connection_nodes), connection_node, struct s_connectable_link) {
    connection_node->final_position_x = (connection_node->offset_x + position_x + (connection_node->width / 2.0));
    connection_node->final_position_y = (connection_node->offset_y + position_y + (connection_node->height / 2.0));
  }
  if (connectable_attributes->draw_rectangle) {
    if (!connectable_attributes->normalized) {
      /* we need to normalize the values stored into the rectangle x and y coordinates, accordingly to the camera */
      for (unsigned int index = 0; index < d_connectable_rectangle_elements; ++index) {
        connectable_attributes->rectangle_x[index] =
          ((double)connectable_attributes->rectangle_x[index] * camera_attributes->screen_w) / camera_attributes->scene_reference_w;
        connectable_attributes->rectangle_y[index] =
          ((double)connectable_attributes->rectangle_y[index] * camera_attributes->screen_h) / camera_attributes->scene_reference_h;
        connectable_attributes->rectangle_x[index] = ((double)connectable_attributes->rectangle_x[index] + camera_attributes->scene_offset_x);
        connectable_attributes->rectangle_y[index] = ((double)connectable_attributes->rectangle_y[index] + camera_attributes->scene_offset_y);
        connectable_attributes->rectangle_x[index] = ((double)connectable_attributes->rectangle_x[index] * camera_attributes->scene_zoom);
        connectable_attributes->rectangle_y[index] = ((double)connectable_attributes->rectangle_y[index] * camera_attributes->scene_zoom);
      }
      connectable_attributes->normalized = d_true;
    }
    f_primitive_fill_polygon(environment_attributes->renderer, connectable_attributes->rectangle_x, connectable_attributes->rectangle_y,
        d_connectable_rectangle_elements, d_connectable_rectangle_R, d_connectable_rectangle_G, d_connectable_rectangle_B,
        d_connectable_rectangle_A);
  }
  d_cast_return(result);
}
d_define_method(connectable, delete)(struct s_object *self, struct s_connectable_attributes *attributes) {
  struct s_connectable_link *current_element;
  while ((current_element = (struct s_connectable_link *)attributes->list_connection_nodes.head)) {
    f_list_delete(&(attributes->list_connection_nodes), (struct s_list_node *)current_element);
    for (unsigned int index = 0; index < d_connectable_max_packets; ++index)
      if (current_element->traveling_packets[index]) {
        d_delete(current_element->traveling_packets[index]);
        current_element->traveling_packets[index] = NULL;
      }
    d_free(current_element);
  }
  return NULL;
}
d_define_class(connectable) {d_hook_method(connectable, e_flag_public, set_generate_traffic),
  d_hook_method(connectable, e_flag_public, set_generate_traffic_speed),
  d_hook_method(connectable, e_flag_public, set_price),
  d_hook_method(connectable, e_flag_public, add_connection_point),
  d_hook_method(connectable, e_flag_public, is_traffic_generation_required),
  d_hook_method(connectable, e_flag_public, get_selected_node),
  d_hook_method(connectable, e_flag_public, is_over),
  d_hook_method_override(connectable, e_flag_public, eventable, event),
  d_hook_method_override(connectable, e_flag_public, drawable, draw),
  d_hook_delete(connectable),
  d_hook_method_tail};
