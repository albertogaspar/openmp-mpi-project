#ifndef OUT_MANAGER_H
#define OUT_MANAGER_H


typedef struct out_tuple{
	long post_id;
	long user_id;
	int num_commenters;
	int score;
}out_tuple;

typedef struct post_score{
	long post_id;
	int score;
}post_score;

typedef struct user_num{
	long user_id;
	int num_commenters;
}user_num;

void out_manager_run();

#endif