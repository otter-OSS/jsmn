#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../myproduct.h"

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
	fp = fopen("./mysource/myproduct.json", "r");
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

static int jsoneq(const char *json, jsmntok_t *tok, const char *s) {
	if (tok->type == JSMN_STRING && (int) strlen(s) == tok->end - tok->start &&
			strncmp(json + tok->start, s, tok->end - tok->start) == 0) {
		return 0;
	}
	return -1;
}

company_t companyType(char *company_name){
	if(strcmp(company_name,"해태")==0) return HAETAE;
	else if(strcmp(company_name,"롯데")==0) return LOTTE;
	else if(strcmp(company_name,"농심")==0) return NONGSHIM;
	else if(strcmp(company_name,"크라운")==0) return CROWN;
	else return -1;
}

char *companyToString(int n){
	char company[20];
	if(n==0) strcpy(company,"해태");
	else if(n==1) strcpy(company,"롯데");
	else if(n==2) strcpy(company,"농심");
	else if(n==3) strcpy(company,"크라운");
	return company;
}

int makeProduct(const char *json, jsmntok_t *t, int tokcount, product_t *p[]){
int i, j, k, start, objectcount=0, count= 0;
	int productcount= 0;

	for(i= 0; i < tokcount ; i++) {

		if(t[i].size>0) {
			if(t[i].type == JSMN_STRING){
				if(t[i+1].type == JSMN_ARRAY || t[i+1].type == JSMN_OBJECT) continue;
					if(jsoneq(json, &t[i], "company") == 0){
						p[productcount] = (product_t *)malloc(sizeof(product_t));
						char temp_company[30];
						strncpy(temp_company, json + t[i+1].start, t[i+1].end - t[i+1].start);
						p[productcount]->company = companyType(temp_company);
						}
					else if(jsoneq(json, &t[i], "name") == 0){
						sprintf(p[productcount]->name,"%.*s",t[i+1].end - t[i+1].start,json + t[i+1].start);
					}
					else if(jsoneq(json, &t[i], "price") == 0){
						char temp_price[20];
						//strncpy(temp_price, json + t[i].start, t[i].end - t[i].start+1);
						sprintf(temp_price, "%.*s",t[i+1].end - t[i+1].start,json + t[i+1].start);
						p[productcount]->price = atoi(temp_price);
					}
					else if(jsoneq(json, &t[i], "count") == 0){
						char temp_count[20];
						sprintf(temp_count, "%.*s",t[i+1].end - t[i+1].start,json + t[i+1].start);
						p[productcount]->count = atoi(temp_count);
						productcount++;
					}
			}
		}
	} return productcount;
}




void printProduct(product_t * p[], int pcount) {
	int i, productNo, totalprice;

	printf("***********************************************\n");
	printf(" 번 호    제품명  제조사   가격    개수   총가격  \n");
	printf("***********************************************\n");
	for(i = 0;i < pcount; i++){
	printf("%3d   ", i+1);
	printf("%10s   ",p[i]->name);
	printf("%8s    ", companyToString(p[i]->company));
	printf("%d      ", p[i]->price);
	printf("%d    ", p[i]->count);
	totalprice = p[i]->price * p[i]->count;
	//sprintf(totalprice, "%d",totalprice2);
	printf("%d   \n",totalprice);

}

}



int main() {
	int i, j;
	int r;
	jsmn_parser p;
	jsmntok_t t[128]; /* We expect no more than 128 tokens */
	char *JSON_STRING = readJSONFile();
	jsmn_init(&p);
	int pcount;
	r = jsmn_parse(&p, JSON_STRING, strlen(JSON_STRING), t, sizeof(t)/sizeof(t[0]));
	if (r < 0) {
		printf("Failed to parse JSON: %d\n", r);
		return 1;
	}

	product_t *product_list[20];
	pcount = makeProduct(JSON_STRING, t, r, product_list);
	printProduct(product_list, pcount);
	printf("pcount = %d\n", pcount);
	return EXIT_SUCCESS;
}
