#include <stdio.h>
#include <stdlib.h>
#include "tetris.h"

static struct tetris *t;

//////////////////////////////////////////////////////////////////
static int t_is_leftest (void)
{
	if ((t->cur_pos.x + t->info[t->cur_te].min_x) <= 0)
		return 1;
	else
		return 0;
}

static int t_is_rightest (void)
{
	if ((t->cur_pos.x + t->info[t->cur_te].max_x) >= T_MAX_X)
		return 1;
	else
		return 0;
}

static int t_is_bottom (void)
{
	if ((t->cur_pos.y + t->info[t->cur_te].max_y) >= T_MAX_Y)
		return 1;
	else
		return 0;
}

void t_clear_table (void)
{
	int i = 0, j = 0;

	for (i = 0; i < T_ROW; i++)
		for (j = 0; j < T_COL; j++)
			t->table[i][j] = 0;
}

static void t_clear (void)
{
	int i = 0;
	int x = 0, y = 0;

	for (i = 0; i < 4; i++) {
		x = t->cur_pos.x + t->info[t->cur_te].xy[i].x;
		y = t->cur_pos.y + t->info[t->cur_te].xy[i].y;
		t->table[y][x] = 0;
	}
}

static int t_is_set_legal (void)
{
	int i = 0;
	int x = 0, y = 0;

	for (i = 0; i < 4; i++) {
		x = t->cur_pos.x + t->info[t->cur_te].xy[i].x;
		y = t->cur_pos.y + t->info[t->cur_te].xy[i].y;
		if (t->table[y][x] != 0)
			return 0;
	}
	return 1;
}

static void t_set (void)
{
	int i = 0;
	int x = 0, y = 0;

	for (i = 0; i < 4; i++) {
		x = t->cur_pos.x + t->info[t->cur_te].xy[i].x;
		y = t->cur_pos.y + t->info[t->cur_te].xy[i].y;
		t->table[y][x] = t->info[t->cur_te].value;
	}
}

static void t_delete_line (int line)
{
	int i = 0, j = 0;

	for (i = 0; i < T_COL; i++) {
		t->table[line][i] = 0;
	}

	if (line == 0)
		return ;
	for (i = --line; i >= 0; i--) {
		for (j = 0; j < T_COL; j++)
			t->table[i+1][j] = t->table[i][j];
	}
	for (j = 0; j < T_COL; j++)
		t->table[0][j] = 0;
}

static int t_check_full_line (void)
{
	int i = 0, j = 0;
	int count = 0;

	for (i = 0; i < T_ROW; i++) {
		count = 0;
		for (j = 0; j < T_COL; j++) {
			if (t->table[i][j] <= 0) {
				count++;
			}
		}
		if (count == 0) {
			return i;
		}
	}

	return -1;
}

////////////////////////////////////////////////////////////////
static int t_is_move_left_legal (void)
{
	int is_legal = 0;

	if (t_is_leftest ())
		return 0;

	t_clear ();
	t->cur_pos.x--;
	is_legal = t_is_set_legal ();
	t->cur_pos.x++;
	t_set ();

	return is_legal;
}

static void t_move_left (void)
{
	t_clear ();
	t->cur_pos.x--;
	t_set ();
}

static int t_is_move_right_legal (void)
{
	int is_legal = 0;

	if (t_is_rightest ())
		return 0;

	t_clear ();
	t->cur_pos.x++;
	is_legal = t_is_set_legal ();
	t->cur_pos.x--;
	t_set ();

	return is_legal;
}
static void t_move_right (void)
{
	t_clear ();
	t->cur_pos.x++;
	t_set ();
}

static int t_is_move_down_legal (void)
{
	int is_legal = 0;

	if (t_is_bottom ())
		return 0;

	t_clear ();
	t->cur_pos.y++;
	is_legal = t_is_set_legal ();
	t->cur_pos.y--;
	t_set ();

	return is_legal;
}
static void t_move_down (void)
{
	t_clear ();
	t->cur_pos.y++;
	t_set ();
}

static int t_is_turn_legal (void)
{
	int i = 0;
	int x = 0, y = 0;

	for (i = 0; i < t->info[t->cur_te].reserved_n; i++) {
		x = t->cur_pos.x + t->info[t->cur_te].reserved[i].x;
		y = t->cur_pos.y + t->info[t->cur_te].reserved[i].y;
		if (t->table[y][x] != 0) {
			return 0;
		}
	}

	return 1;
}

static void t_turn (void)
{
	int offset = 0;

	t_clear ();

	offset = t->cur_te % 4;
	offset += 1;
	if (offset >= 4) {
		offset = 0;
	}

	t->cur_te = t->base_te * 4 + offset;

	t_set ();
}

void t_check_and_score (void)
{
	int i = 0;

	t->del_line_count = 0;
	while (1) {
		i = t_check_full_line ();
		if (i < 0) {
			break;
		}

		t_delete_line (i);
		t->del_line_count += 1;
	}

	if (t->del_line_count == 0) {

	} else if (t->del_line_count == 1) {
		t->score += 100;
	} else if (t->del_line_count == 2) {
		t->score += 300;
	} else if (t->del_line_count == 3) {
		t->score += 500;
	} else if (t->del_line_count == 4) {
		t->score += 800;
	}
}

void t_te_create (void)
{
	t->cur_te = rand() % 28;
	t->next_te = t->cur_te;
	t->base_te = t->cur_te / 4;
}

void t_te_create_next (void)
{
	t->cur_te = t->next_te;
	t->next_te = rand() % 28;
	t->base_te = t->cur_te / 4;
}

int t_te_out (void)
{
	t->cur_pos.x = (T_COL / 2) - 2;
	t->cur_pos.y = 0 - t->info[t->cur_te].min_y;

	if (t_is_set_legal ()) {
		t_set ();
		return 0;
	} else {
		return -1;
	}
}
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

void t_key_left (void)
{
	if (t_is_move_left_legal ()) {
		t_move_left ();
	}
}

void t_key_right (void)
{
	if (t_is_move_right_legal ()) {
		t_move_right ();
	}
}

int t_key_down (void)
{
	if (t_is_move_down_legal ()) {
		t_move_down();
		//t_check_and_score ();
		return 0;
	} else {
		return -1;
	}
}

void t_key_turn (void)
{
	if (t_is_turn_legal ()) {
		t_turn ();
	}
}

//////////////////////////////////////////////////////////////////////

int t_get_score (void)
{
	return t->score;
}

P_NEXT t_get_next (void)
{
	return &t->info[t->next_te].xy;
}

P_TABLE t_get_table (void)
{
	return &t->table[0];
}

struct tetris *t_get_t (void)
{
	return t;
}

void t_exit (void)
{
	free (t);
}

void t_init (void)
{
	#include "tetriminos.ini"
	int i = 0, j = 0, k = 0, n = 0;

	t = (struct tetris *) malloc (sizeof (struct tetris));

	// clear table
	for (i = 0; i < T_ROW; i++)
		for (j = 0; j < T_COL; j++)
			t->table[i][j] = 0;

	// 
	t->score = 0;
	t->base_te = 0;
	t->cur_te = 0;
	t->next_te = 0;
	t->cur_pos.x = 0;
	t->cur_pos.y = 0;

	// set tetrimino_value
	for (i = 0; i < 28; i++)
		t->info[i].value = 1;

	// xy[4], max_x, min_x, max_y, min_y
	for (i = 0; i < 28; i++) {
		n = 0;
		t->info[i].max_x = 0;
		t->info[i].min_x = 3;
		t->info[i].max_y = 0;
		t->info[i].min_y = 3;
		for (j = 0; j < 4; j++) {
		for (k = 0; k < 4; k++) {
		if (tetriminos[i][j][k] > 0) {
			t->info[i].xy[n].x = k;
			t->info[i].xy[n].y = j;
			n++;
			if (k > t->info[i].max_x) {
				t->info[i].max_x = k;
			}
			if (k < t->info[i].min_x) {
				t->info[i].min_x = k;
			}
			if (j > t->info[i].max_y) {
				t->info[i].max_y = j;
			}
			if (j < t->info[i].min_y) {
				t->info[i].min_y = j;
			}
		}}}
	}

	// reserved[16];
	for (i = 0; i < 28; i++) {
		n = 0;
		for (j = 0; j < 4; j++) {
		for (k = 0; k < 4; k++) {
		if (tetriminos[i][j][k] == -1) {
			t->info[i].reserved[n].x = k;
			t->info[i].reserved[n].y = j;
			n++;
		}}}
		t->info[i].reserved_n = n;
	}
}
