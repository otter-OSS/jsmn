#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../OSS_21700525.h"



char *readjsonfile() { //코드 내에 파일명 삽입하였음.
	char oneline[250];
  char fileName[100];
	char *js = (char *)malloc(sizeof(oneline));
	int count=0;
	FILE *fp;
	fp = fopen("./mysource/cars.json", "r"); //위의 파일명을 경로와 함께 읽어들여 읽기모드로 fopen한다.
	if(fp == NULL){//일치하는 파일이 없을 때
		printf("파일이 없음");
		return NULL;
	}

	while(1){
		fgets(oneline, sizeof(oneline),fp);//한줄씩 읽어들인다.
		if(feof(fp)) break;//파일을 모두 읽으면 break;

		count += strlen(oneline);//count에 읽어들인 한줄씩 더해간다.
		js = (char *)realloc(js, count+1); //count+1(널문자) 사이즈로 js를 realloc 해준다.
		strcat(js, oneline); //방금 읽은 한줄을 js에 이어준다.
	}
	fclose(fp);
	return js;
}

static int jsoneq(const char *json, jsmntok_t *tok, const char *s) {
	if (tok->type == JSMN_STRING && (int) strlen(s) == tok->end - tok->start &&
			strncmp(json + tok->start, s, tok->end - tok->start) == 0) {
		return 0;
	}
	return -1;
}



int makecarlist(const char *json, jsmntok_t *t, int tokcount, car_t *c[]){
int i, carcount= 0;

	for(i= 0; i < tokcount ; i++) {

		if(t[i].size>0) {
			if(t[i].type == JSMN_STRING){
				if(t[i+1].type == JSMN_ARRAY || t[i+1].type == JSMN_OBJECT) continue;
					if(jsoneq(json, &t[i], "model") == 0){ //simple.c에서 사용된 jsoneq함수를 이용하여 key 토큰을 찾아낸다.
						c[carcount] = (car_t *)malloc(sizeof(car_t));
						//"model"은 car에 관한 정보의 첫번째 key이므로 메모리를 할당한다.
						sprintf(c[carcount]->model, "%.*s",t[i+1].end - t[i+1].start,json + t[i+1].start);
						//model 다음 토큰(valude)을 c[carcount]->model에 넣어준다.
						}
					else if(jsoneq(json, &t[i], "maker") == 0){
						sprintf(c[carcount]->maker,"%.*s",t[i+1].end - t[i+1].start,json + t[i+1].start);
						//maker 다음 토큰을 c[carcount]->maker에 넣어준다.
					}
					else if(jsoneq(json, &t[i], "year") == 0){
						char temp_year[20];
						sprintf(temp_year, "%.*s",t[i+1].end - t[i+1].start,json + t[i+1].start);
						//maker 다음 토큰을 temp_year에 넣어주고
						c[carcount]->year= atoi(temp_year); //atoi함수를 사용하여 string을 int로 변환해준다.
					}
					else if(jsoneq(json, &t[i], "gas") == 0){
						sprintf(c[carcount]->gastype, "%.*s",t[i+1].end - t[i+1].start,json + t[i+1].start);
						//maker 다음 토큰을 c[carcount]->gastype에 넣어준다.
						carcount++; //gas는 car에 관한 정보의 마지막 key이므로 carcount를 하나 늘려준다.
					}
			}
		}
	} return carcount;
}




void printcarlist(car_t *c[], int carcount) {
	int i;
	printf("*************************************************\n");
	printf(" 번 호    모델명    제조사   제조년도    연료타입  \n");
	printf("*************************************************\n");
	for(i = 0;i < carcount; i++){
		//carcount만큼 loop을 돌려 하나씩 출력해준다.
		printf("%3d   ", i+1);
		printf("%10s   ",c[i]->model);
		printf("%8s    ", c[i]->maker);
		printf("%d      ", c[i]->year);
		printf("%8s    ", c[i]->gastype);
		printf("\n");
	}
}



int main() {
	int i, j;
	int r;
	jsmn_parser p;
	jsmntok_t t[128]; /* We expect no more than 128 tokens */
	char *JSON_STRING = readjsonfile();
	jsmn_init(&p);
	int carcount;
	r = jsmn_parse(&p, JSON_STRING, strlen(JSON_STRING), t, sizeof(t)/sizeof(t[0]));
	if (r < 0) {
		printf("Failed to parse JSON: %d\n", r);
		return 1;
	}

	car_t *carlist[5];
	carcount = makecarlist(JSON_STRING, t, r, carlist);
	printcarlist(carlist, carcount);
	return EXIT_SUCCESS;
}
