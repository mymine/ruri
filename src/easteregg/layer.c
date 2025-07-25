// SPDX-License-Identifier: MIT
/*
 *
 * This file is part of ruri, with ABSOLUTELY NO WARRANTY.
 *
 * MIT License
 *
 * Copyright (c) 2024 Moe-hacker
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 *
 */
#include "include/nekofeng.h"
// NOLINTBEGIN
#ifndef RURI_CORE_ONLY
/*
 * This file provides the function
 * nekofeng_add_action(), nekofeng_play_action(), nekofeng_playback_action(), and nekofeng_free_action().
 *
 * Usage:
 *
 *  struct NEKOFENG_ACTION *action = NULL;
 *  action = nekofeng_add_action(action, 11, 4,
 *              "\n\033[0m vedvhfkhbfuweh  \n\n");
 *  action = nekofeng_add_action(action, 11, 4,
 *              "\n\033[0m vedvhfkhbfuweh  \n\n");
 *  nekofeng_play_action(action, inr, keep);
 *  nekofeng_playback_action(action, inr, keep);
 *  nekofeng_free_action(action);
 *
 * Do not care how it works, because I forgot,
 * and I am too lazy to read the code.
 *
 */
static void clear_layer(struct NEKOFENG_LAYER *_Nonnull layer)
{
	nekofeng_spin_lock(&nekofeng_lock);
	int y_offset = 0;
	int x_offset = 0;
	printf("\033[%dH", nekofeng_y + layer->y_offset);
	printf("\033[%dG", nekofeng_x + layer->x_offset);
	for (size_t i = 0; i < nekofeng_strlen(layer->layer); i++) {
		// The next line.
		if (layer->layer[i] == U'\n') {
			y_offset++;
			x_offset = 0;
			printf("\033[%dH", nekofeng_y + layer->y_offset + y_offset);
			printf("\033[%dG", nekofeng_x + layer->x_offset);
			continue;
		}
		// Color.
		if (layer->layer[i] == U'\033') {
			for (size_t j = i; j < nekofeng_strlen(layer->layer); j++) {
				if (layer->layer[j] == U'm') {
					i = j;
					break;
				}
			}
			continue;
		}
		// Skip space.
		if (layer->layer[i] != U' ') {
			printf("\033[%dG", nekofeng_x + layer->x_offset + x_offset);
			printf(" ");
		}
		x_offset++;
	}
	fflush(stdout);
	nekofeng_spin_unlock(&nekofeng_lock);
	usleep(10000);
}
static void print_layer(struct NEKOFENG_LAYER *_Nonnull layer)
{
	nekofeng_spin_lock(&nekofeng_lock);
	int y_offset = 0;
	printf("\033[%dH", nekofeng_y + layer->y_offset);
	printf("\033[%dG", nekofeng_x + layer->x_offset);
	for (size_t i = 0; i < nekofeng_strlen(layer->layer); i++) {
		// The next line.
		if (layer->layer[i] == U'\n') {
			y_offset++;
			printf("\033[%dH", nekofeng_y + layer->y_offset + y_offset);
			printf("\033[%dG", nekofeng_x + layer->x_offset);
			continue;
		}
		// '#' means a ' ' to cover the layer under it.
		if (layer->layer[i] == U'#') {
			printf(" ");
		} // Skip space.
		else if (layer->layer[i] != U' ') {
			char character[64] = { '\0' };
			mbstate_t state = { 0 };
			size_t len = c32rtomb(character, layer->layer[i], &state);
			if (len == (size_t)-1) {
				perror("c32rtomb");
				nekofeng_spin_unlock(&nekofeng_lock);
				return;
			}
			character[len] = '\0';
			printf("%s", character);
		} else {
			printf("\033[1C\033[?25l");
		}
	}
	printf("\033[0m");
	fflush(stdout);
	nekofeng_spin_unlock(&nekofeng_lock);
	usleep(10000);
}
void nekofeng_play_action(struct NEKOFENG_ACTION *_Nonnull action, useconds_t inr, unsigned int keep)
{
	struct NEKOFENG_ACTION **p = &action;
	while ((*p) != NULL) {
		print_layer((*p)->layer);
		usleep(inr);
		if ((*p)->next == NULL) {
			sleep(keep);
		}
		clear_layer((*p)->layer);
		p = &((*p)->next);
	}
}
void nekofeng_playback_action(struct NEKOFENG_ACTION *_Nonnull action, useconds_t inr, unsigned int keep)
{
	struct NEKOFENG_ACTION **p = &action;
	while ((*p)->next != NULL) {
		p = &((*p)->next);
	}
	while ((*p) != NULL) {
		print_layer((*p)->layer);
		usleep(inr);
		if ((*p)->prior == NULL) {
			sleep(keep);
		}
		clear_layer((*p)->layer);
		p = &((*p)->prior);
	}
}
void nekofeng_free_action(struct NEKOFENG_ACTION *_Nonnull action)
{
	struct NEKOFENG_ACTION *p = action;
	while (p != NULL) {
		struct NEKOFENG_ACTION *t = p;
		p = p->next;
		free(t->layer->layer);
		free(t->layer);
		free(t);
	}
}
struct NEKOFENG_ACTION *nekofeng_add_action(struct NEKOFENG_ACTION *_Nonnull action, int x_offset, int y_offset, char32_t *_Nonnull layer)
{
	struct NEKOFENG_ACTION **p = &action;
	struct NEKOFENG_ACTION *prior = action;
	while (*p != NULL) {
		prior = *p;
		p = &((*p)->next);
	}
	(*p) = malloc(sizeof(struct NEKOFENG_ACTION));
	(*p)->layer = malloc(sizeof(struct NEKOFENG_LAYER));
	(*p)->layer->x_offset = x_offset;
	(*p)->layer->y_offset = y_offset;
	(*p)->layer->layer = nekofeng_strdup(layer);
	(*p)->next = NULL;
	(*p)->prior = prior;
	return action;
}
#endif
// NOLINTEND