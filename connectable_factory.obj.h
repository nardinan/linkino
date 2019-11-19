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
#ifndef linkino_connectable_factory_h
#define linkino_connectable_factory_h
#include "connectable.obj.h"
#define d_connectable_factory_connections 10
#define d_connectable_factory_button_width 150.0
#define d_connectable_factory_button_height 30.0
#define d_connectable_factory_alpha 100
#define d_connectable_factory_mask_step 2
typedef struct s_connectable_factory_template { d_list_node_head;
  struct s_object *stream;
  struct s_object *uiable_button;
  struct s_object *drawable_icon;
  char description[d_string_buffer_size], title[d_string_buffer_size];
  size_t connections;
  double offsets_x[d_connectable_factory_connections], offsets_y[d_connectable_factory_connections], position_x, position_y, price;
  t_boolean generate_traffic, filter_spam, shape_traffic;
} s_connectable_factory_template;
d_declare_class(connectable_factory) {
  struct s_attributes head;
  struct s_list list_templates;
  struct s_connectable_factory_template *active_template;
  struct s_connectable_link *active_node;
  struct s_object *active_connectable;
  struct s_object *delete_connectable;
  struct s_object *array_connectable_instances;
  struct s_object *environment;
  struct s_object *ui_factory;
  struct s_object *ui_container_sell_confirmation;
  struct s_object *ui_container_bank_balance;
  struct s_object *ui_label_bank_balance;
  t_boolean connector_selected, ui_container_sell_confirmation_visible;
  double current_credit, mask_credit_red, mask_credit_green, mask_credit_blue;
} d_declare_class_tail(connectable_factory);
struct s_connectable_factory_attributes *p_connectable_factory_alloc(struct s_object *self);
extern struct s_object *f_connectable_factory_new(struct s_object *self, struct s_object *ui_factory, struct s_object *environment);
d_declare_method(connectable_factory, add_connectable_template)(struct s_object *self, struct s_object *stream, const char *title, const char *description,
  double *offsets_x, double *offsets_y, size_t connections, double price, t_boolean generate_traffic, t_boolean filter_spam, t_boolean shape_traffic);
d_declare_method(connectable_factory, set_connector_selected)(struct s_object *self, t_boolean selected);
d_declare_method(connectable_factory, set_credit)(struct s_object *self, double credit);
d_declare_method(connectable_factory, get_selected_node)(struct s_object *self);
d_declare_method(connectable_factory, is_traffic_generation_required)(struct s_object *self);
d_declare_method(connectable_factory, click_received_create)(struct s_object *self, struct s_connectable_factory_template *template);
d_declare_method(connectable_factory, click_received_sell)(struct s_object *self);
d_declare_method(connectable_factory, click_received_cancel)(struct s_object *self);
d_declare_method(connectable_factory, event)(struct s_object *self, struct s_object *environment, SDL_Event *current_event);
d_declare_method(connectable_factory, draw)(struct s_object *self, struct s_object *environment);
d_declare_method(connectable_factory, reset)(struct s_object *self);
d_declare_method(connectable_factory, delete)(struct s_object *self, struct s_connectable_factory_attributes *attributes);
#endif
