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
#include "miranda.h"
/* default values, replaced by the values into the configuration file */
double v_linkino_width_window = 1280, v_linkino_height_window = 720, v_linkino_width_reference = 1280,
  v_linkino_height_reference = 720, v_linkino_scale_factor = 1.0, v_linkino_effects_volume = 1.0, v_linkino_music_volume = 1.0;
t_boolean v_linkino_fullscreen = d_false;
