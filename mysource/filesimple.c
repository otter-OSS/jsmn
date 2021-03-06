#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../jsmn.h"

/*
 * A small example of jsmn parsing when JSON structure is known and number of
 * tokens is predictable.
 */

char *readJSONFile() {
	char oneline[250];

	char *js = (char *)malloc(sizeof(oneline));
	int count=0;
	FILE *fp;

	fp = fopen("./mysource/data2.json", "r");
	if(fp == NULL){
		printf("파일이 없음");
		return NULL;
	}

	while(1){
		fgets(oneline, sizeof(oneline),fp);
		if(feof(fp)) break;

		count += strlen(oneline);
		js = (char *)realloc(js, count+1);
		strcat(js, oneline);
	}
	fclose(fp);
	return js;
}

int jsoneq(const char *json, jsmntok_t *tok1, jsmntok_t *tok2) {
 	//printf("")
	if (tok1->type == tok2->type && (tok1->end - tok1->start) == (tok2->end - tok2->start)
		&& strncmp(json + tok1->start, json + tok2->start, tok1->end - tok1->start) == 0)	 return 0;
	return -1;
}

void jsonNameList(char *jsonstr, jsmntok_t *t, int tokcount, int *nameTokIndex) {
	int i, count= 0;
	int nextToKey= 0;
	int keyTokenSize= 0;
	for(i=1; i < tokcount ; i++) {
		if(t[i].type == JSMN_STRING && t[i].parent == 0) {
			nameTokIndex[count] = i;
			count++;
			keyTokenSize = t[i].size;
		}
	}
//	printf("count : %d\n", count);
}

void printNameList(char *jsonstr, jsmntok_t *t, int *nameTokIndex){
	int i;
	printf("***** Name List *****\n");
	for(i=0; nameTokIndex[i]!=0 ; i++) printf(" [NAME %d] %.*s\n" ,i+1 ,t[nameTokIndex[i]].end-t[nameTokIndex[i]].start,
	jsonstr + t[nameTokIndex[i]].start);
}

int *showFirstValueofLists(char *jsonstr, jsmntok_t *t, int *nameTokIndex, int *firstIndexList){
	int i = 0, count= 0;
	int *temp = (int *)malloc(sizeof(int));
	printf("***** Object List *)****\n");
	printf(" [NAME %d] %.*s\n" , i+1 ,t[nameTokIndex[i]+1].end-t[nameTokIndex[i]+1].start, jsonstr + t[nameTokIndex[i]+1].start); //가장 첫번째 value를 출력.
	temp[0] = 0;
	count++;
	for(i= 1; nameTokIndex[i]!=0 ; i++ ) {
		if(jsoneq(jsonstr, &t[nameTokIndex[0]], &t[nameTokIndex[i]])== 0) {
			//loop속에서 현재 key가 첫번째 value의 key와 동일한가 jsoneq함수를 이용하여 비교
			temp = (int *)realloc(temp, sizeof(int) * (count+1));
			temp[count] = i;
			count++;
			printf(" [NAME %d] %.*s\n" ,count ,t[nameTokIndex[i]+1].end-t[nameTokIndex[i]+1].start, jsonstr + t[nameTokIndex[i]+1].start);
		//true이면 count를 늘리고 출력.
		}
		temp = (int *)realloc(temp, sizeof(int) * (count+1));
		temp[count] = i+1;
	}
	firstIndexList = temp;
	return firstIndexList;
}

void printObject(char *jsonstr, jsmntok_t *t, int *nameTokIndex, int *firstIndexList){
	int num;
	printf("원하는 Object 번호 입력 (Exit:0) : ");
	scanf("%d", &num);
	int i;
	printf("***** Object List *****\n");
	for(i=firstIndexList[num-1] ; i< firstIndexList[num]; i++){
			printf("	[ %.*s ] ",t[nameTokIndex[i]].end-t[nameTokIndex[i]].start,	jsonstr + t[nameTokIndex[i]].start);
			printf(" %.*s\n", t[nameTokIndex[i]+1].end-t[nameTokIndex[i]+1].start,	jsonstr + t[nameTokIndex[i]+1].start);
	}
}

void selectNameList(char *jsonstr, jsmntok_t *t, int *nameTokIndex){
	int num;
	while(1){
		printf("Select Name's no (exit:0) >> ");
		scanf("%d", &num);
		if(num==0) break;
		int  i;
		printf("[NAME %d] %.*s\n",num ,t[nameTokIndex[num-1]].end-t[nameTokIndex[num-1]].start,
		jsonstr + t[nameTokIndex[num-1]].start);
		int nameTokNextIndex = nameTokIndex[num-1]+1;
		if(t[nameTokNextIndex].type==JSMN_STRING){
			printf("%.*s\n",t[nameTokNextIndex].end-t[nameTokNextIndex].start,jsonstr + t[nameTokNextIndex].start);
			}

		else if(t[nameTokNextIndex].type ==JSMN_OBJECT){
			int objectEnd = t[nameTokNextIndex].end;
			i= nameTokNextIndex + 1;
			while(t[i].size <= 1 && t[i].start < objectEnd){
				if(t[i].size != 0) printf("%.*s: ",t[i].end-t[i].start, jsonstr + t[i].start);
					else printf("%.*s,\n",t[i].end-t[i].start, jsonstr + t[i].start);
					i++;
			}
		}
		else if(t[nameTokNextIndex].type ==JSMN_ARRAY){
			int arrayEnd = t[nameTokNextIndex].end;
			i= nameTokNextIndex +1;
			while(t[i].size == 0 && t[i].start < arrayEnd){
				printf("%.*s: ",t[i].end-t[i].start, jsonstr + t[i].start);
				i++;
			}
		}
			printf("\n");
		}
	}

int main() {
	int i;
	int r;
	jsmn_parser p;
	jsmntok_t t[128]; /* We expect no more than 128 tokens */
	char *JSON_STRING = readJSONFile();
	jsmn_init(&p);

	r = jsmn_parse(&p, JSON_STRING, strlen(JSON_STRING), t, sizeof(t)/sizeof(t[0]));
	if (r < 0) {
		printf("Failed to parse JSON: %d\n", r);
		return 1;
	}

	int nameTokIndex[100]={0};
	jsonNameList(JSON_STRING, t, r, nameTokIndex);
	printNameList(JSON_STRING, t, nameTokIndex);
	int *firstIndexList = NULL;
	firstIndexList = showFirstValueofLists(JSON_STRING, t, nameTokIndex, firstIndexList);
	//selectNameList(JSON_STRING, t, nameTokIndex);
	printObject(JSON_STRING, t, nameTokIndex, firstIndexList);

	return EXIT_SUCCESS;

}
