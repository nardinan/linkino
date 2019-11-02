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
#ifndef linkino_miranda_h
#define linkino_miranda_h
#include <miranda/ground.h>
#include <miranda/objects/objects.h>
#include <miranda/objects/io/io.h>
#include <miranda/objects/geometry/geometry.h>
#include <miranda/objects/media/media.h>
#include <miranda/objects/media/cameras/cameras.h>
#include <miranda/objects/media/ui/ui.h>
#include <miranda/objects/network/network.h>
#include <miranda/endian.local.h>
#define d_linkino_game_configuration "linkino_config"
#define d_linkino_ui_configuration "linkino_ui"
extern double v_linkino_width_window, v_linkino_height_window, v_linkino_width_reference, v_linkino_height_reference,
  v_linkino_scale_factor, v_linkino_effects_volume, v_linkino_music_volume;
extern t_boolean v_linkino_fullscreen;
#endif

