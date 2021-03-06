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
#ifndef linkino_loader_h
#define linkino_loader_h
#include "miranda.h"
#define d_linkino_title "linkino"
#define d_linkino_resources "./data"
#define d_linkino_resources_default_png    "./data/placeholders/default_png.png"
#define d_linkino_resources_default_ttf    "./data/placeholders/default_ttf.ttf"
#define d_linkino_resources_default_json   "./data/placeholders/default_json.json"
#define d_linkino_resources_default_ogg    "./data/placeholders/default_ogg.wav"
#define d_linkino_resources_default_lisp   "./data/placeholders/default_lisp.lisp"
#define d_linkino_resources_configuration  "./data/linkino_config.json"
d_declare_class(loader) {
  struct s_attributes head;
  struct s_object *environment;
  struct s_object *resources_png, *resources_ttf, *resources_ogg, *resources_json, *resources_lisp;
  struct s_object *media_factory;
  struct s_object *ui_factory;
} d_declare_class_tail(loader);
struct s_loader_attributes *p_loader_alloc(struct s_object *self);
extern struct s_object *f_loader_new(struct s_object *self, struct s_object *environment);
d_declare_method(loader, run)(struct s_object *self);
d_declare_method(loader, delete)(struct s_object *self, struct s_loader_attributes *attributes);
#endif
