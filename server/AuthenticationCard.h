
typedef struct authenticationCard{
	char* ip;
	int hour;
	int minute;
	int second;
	struct authentication* next;
}CARD;

typedef struct authenticationCase{
	int count;
	struct authentication* head;
	struct authentication* tail;
}LIST;
