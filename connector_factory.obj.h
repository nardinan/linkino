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
#ifndef linkino_connector_factory_h
#define linkino_connector_factory_h
#include "connector.obj.h"
#include "connectable.obj.h"
d_declare_class(connector_factory) {
  struct s_attributes head;
  struct s_object *active_connector;
  struct s_object *array_of_connectors;
  struct s_object *drawable;
  t_boolean approve_drop;
  struct s_connectable_link *source_link, *destination_link;
} d_declare_class_tail(connector_factory);
struct s_connector_factory_attributes *p_connector_factory_alloc(struct s_object *self);
extern struct s_object *f_connector_factory_new(struct s_object *self, struct s_object *drawable);
d_declare_method(connector_factory, set_drop)(struct s_object *self, t_boolean approve_drop, struct s_connectable_link *link);
d_declare_method(connector_factory, get_connector_with_source)(struct s_object *self, struct s_connectable_link *link);
d_declare_method(connector_factory, get_connector_with_destination)(struct s_object *self, struct s_connectable_link *link);
d_declare_method(connector_factory, event)(struct s_object *self, struct s_object *environment, SDL_Event *current_event);
d_declare_method(connector_factory, draw)(struct s_object *self, struct s_object *environment);
d_declare_method(connector_factory, delete)(struct s_object *self, struct s_connector_factory_attributes *attributes);
#endif
