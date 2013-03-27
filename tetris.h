#ifndef _TETRIS_H
#define _TETRIS_H

#define T_COL 10
#define T_ROW 20
#define T_MAX_X (T_COL - 1)
#define T_MAX_Y (T_ROW - 1)

typedef enum tetrimino {
	I, I_90, I_180, I_270,
	J, J_90, J_180, J_270,
	L, L_90, L_180, L_270,
	O, O_90, O_180, O_270,
	S, S_90, S_180, S_270,
	T, T_90, T_180, T_270,
	Z, Z_90, Z_180, Z_270
} TETRIMINO;

typedef struct coordinate {
	int x;
	int y;
} COORDINATE;

typedef struct tetrimino_info {
	int value;
	COORDINATE xy[4];
	int max_x;
	int min_x;
	int max_y;
	int min_y;
	COORDINATE reserved[16];
	int reserved_n;
} TETRIMINO_INFO;

typedef struct tetris {
	int table[T_ROW][T_COL];
	int score;
	int del_line_count;
	TETRIMINO base_te;
	TETRIMINO cur_te;
	TETRIMINO next_te;
	COORDINATE cur_pos;
	TETRIMINO_INFO info[28];
} TETRIS;

typedef int (*P_TABLE)[T_COL];
typedef COORDINATE (*P_NEXT)[4];

////////////////////////////////////////////////////////////////
extern int t_te_out (void);
extern void t_te_create (void);
extern void t_te_create_next (void);

extern void t_key_left (void);
extern void t_key_right (void);
extern int t_key_down (void);
extern void t_key_turn (void);

extern void t_check_and_score (void);

extern int t_get_score (void);
extern P_TABLE t_get_table (void);
extern P_NEXT t_get_next (void);
extern struct tetris *t_get_t (void);

extern void t_init (void);
extern void t_exit (void);

extern void t_clear_table (void);

#endif
