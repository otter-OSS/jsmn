#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../productlist.h"

/*
 * A small example of jsmn parsing when JSON structure is known and number of
 * tokens is predictable.
 */

char *readJSONFile() {
	char oneline[250];
  char fileName[100];
	char *js = (char *)malloc(sizeof(oneline));
	int count=0;
	FILE *fp;
	fp = fopen("./mysource/data4.json", "r");
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

int jsonNameList(char *jsonstr, jsmntok_t *t, int tokcount, NameTokenInfo *nameTokenInfo){
	int i, j, k, start, objectcount=0, count= 0;
	int *superTokSize= (int *)malloc(sizeof(int) * tokcount);
	int *countSize= (int *)malloc(sizeof(int) * tokcount);
	int lastSuperTokIndex= 0;

	for(i=0; i< tokcount; i++) countSize[i]= 0;

	//nameTokenInfo =(NameTokenInfo *)malloc(sizeof(NameTokenInfo));

	for(k=0; k< tokcount; k++){
	superTokSize[lastSuperTokIndex]= t[k].size;
	countSize[lastSuperTokIndex]= 0;
		if(t[k+1].type == JSMN_STRING){
			if(t[k+2].type == JSMN_ARRAY){
				superTokSize[lastSuperTokIndex]= t[k+2].size;
				countSize[lastSuperTokIndex]= 0;
				lastSuperTokIndex++;
				start = k+3;
				break;
				}
			start = k+1;
			break;
		  }
		lastSuperTokIndex++;
	}

	for(i= start; i < tokcount ; i++) {
		for(k= lastSuperTokIndex; countSize[k] == superTokSize[k]; k--){
				//현재 super 토큰의 사이즈만큼 토큰을 check 했
			countSize[lastSuperTokIndex]= 0;
		 	superTokSize[lastSuperTokIndex]= -1;
			lastSuperTokIndex--;
		}

		countSize[lastSuperTokIndex]++;

		if(t[i].size>0) {
			if(t[i].type == JSMN_STRING){
				if(lastSuperTokIndex==start-1){
				nameTokenInfo[count].tokindex = i;

				if(jsoneq(jsonstr, &t[nameTokenInfo[0].tokindex], &t[nameTokenInfo[count].tokindex])== 0)
					if(count!=0) objectcount++;
				nameTokenInfo[count].objectindex = objectcount;
				count++;
				}
			}

			lastSuperTokIndex++;
			superTokSize[lastSuperTokIndex]= t[i].size;
			countSize[lastSuperTokIndex]= 0;

			}

	}
}

int getTokenIndex (char *jsonstr, jsmntok_t *t, NameTokenInfo *nameTokenInfo){
	int inputObjectNo,i;
	char inputName[100];
 	printf("name 입력: ");
	fgets(inputName,sizeof(inputName),stdin);
	printf("Object 번호 입력 : ");
	scanf("%d",&inputObjectNo);
//	printf("sizeof(nameTokIndex): %d\n")
	for(i= 0; i< 100; i++){
		if(inputObjectNo == nameTokenInfo[i].objectindex){
			if(strncmp(jsonstr + t[nameTokenInfo[i].tokindex].start, inputName, t[nameTokenInfo[i].tokindex].end - t[nameTokenInfo[i].tokindex].start) == 0){
				return nameTokenInfo[i].tokindex;
				}
			}
		}
}


void printObject(char *jsonstr, jsmntok_t *t, NameTokenInfo *nameTokenInfo){
	int i, objectNo;
	char price[20]="";
	char count[20]="";
	char totalprice[20]="";
	int price2=0, count2=0, totalprice2=0;
	for(i =0; i<50; i++) printf("*");
	printf("\n");
	printf("번 호    제품명  제조사   가격    개수   총가격  \n");
	for(i =0; i<50; i++) printf("*");
	printf("\n");
	for(i = 0;i < 4; i++){
	printf("%3d      ", nameTokenInfo[4*i].objectindex);
	printf("%.*s   ",t[nameTokenInfo[4*i+1].tokindex+1].end-t[nameTokenInfo[4*i+1].tokindex+1].start, jsonstr + t[nameTokenInfo[4*i+1].tokindex+1].start);
	printf("%.*s    ",t[nameTokenInfo[4*i].tokindex+1].end-t[nameTokenInfo[4*i].tokindex+1].start, jsonstr + t[nameTokenInfo[4*i].tokindex+1].start);

	printf("%.*s      ",t[nameTokenInfo[4*i+2].tokindex+1].end-t[nameTokenInfo[4*i+2].tokindex+1].start, jsonstr + t[nameTokenInfo[4*i+2].tokindex+1].start);
	strncpy(price,jsonstr + t[nameTokenInfo[4*i+2].tokindex+1].start, t[nameTokenInfo[4*i+2].tokindex+1].end-t[nameTokenInfo[4*i+2].tokindex+1].start);
	price2 = atoi(price);

	printf("%.*s    ",t[nameTokenInfo[4*i+3].tokindex+1].end-t[nameTokenInfo[4*i+3].tokindex+1].start, jsonstr + t[nameTokenInfo[4*i+3].tokindex+1].start);
	strncpy(count,jsonstr + t[nameTokenInfo[4*i+3].tokindex+1].start, t[nameTokenInfo[4*i+3].tokindex+1].end-t[nameTokenInfo[4*i+3].tokindex+1].start);
	count2 = atoi(count);

	totalprice2 = price2 * count2;
	//sprintf(totalprice, "%d",totalprice2);
	printf("%d   \n",totalprice2);

	}


}



int main() {
	int i, j;
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
	NameTokenInfo nameTokenInfo[100];

	jsonNameList(JSON_STRING, t, r, nameTokenInfo);
	int tokenindex = getTokenIndex(JSON_STRING, t, nameTokenInfo);
	printObject(JSON_STRING, t, nameTokenInfo);

	return EXIT_SUCCESS;
}
