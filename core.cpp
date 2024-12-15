#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include "AVL_Index.cpp"
using namespace std;


// ��CSV�ļ���ȡ���ݵ�RowSet�ĺ���ʵ��
RowSet csvToRowSet(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    RowSet rowSet;
    rowSet.rows = NULL;
    rowSet.row_count = 0;

    char line[1024];
    while (fgets(line, sizeof(line), file)) {
        // �����ŷָ�����
        char copy[1000];
        strcpy(copy,line);
        char *token = strtok(copy, ",");

        Row row;
        row.cells = NULL;
        row.cell_count = 0;

        // ͳ��Ԫ������
        int element_count = 0;
        while (token) {
            element_count++;
            token = strtok(NULL, ",");
        }

        // Ϊ���е�Cell��������ڴ�
        row.cells = (Cell *)malloc(element_count * sizeof(Cell));
        if (row.cells == NULL) {
            perror("Memory allocation failed");
            exit(EXIT_FAILURE);
        }

        // ���¿�ʼ������������
        token = strtok(line, ",");
        int cell_index = 0;
        while (token) {
            // ����������
            strcpy(row.cells[cell_index].type, "char*");
            row.cells[cell_index].size = strlen(token);
            row.cells[cell_index].value = (void *)strdup(token);

            cell_index++;
            token = strtok(NULL, ",\n");
        }

        row.cell_count = element_count;

        // ��Row��ӵ�RowSet
        rowSet.row_count++;
        rowSet.rows = (Row *)realloc(rowSet.rows, rowSet.row_count * sizeof(Row));
        rowSet.rows[rowSet.row_count - 1] = row;
    }

    fclose(file);
    return rowSet;
}


// ��ӡRowSet�����ݵĺ���ʵ��
void printRowSet(RowSet rowSet) {
    for (int i = 0; i < rowSet.row_count; i++) {
        Row row = rowSet.rows[i];
        for (int j = 0; j < row.cell_count; j++) {
            Cell cell = row.cells[j];
            if (strcmp(cell.type, "int") == 0) {
                printf("%d ", *(int *)cell.value);
            } else if (strcmp(cell.type, "char*") == 0) {
                printf("|%10s| ", (char *)cell.value);
            }
        }
        printf("\n");
    }

    // �ͷ��ڴ�
    for (int i = 0; i < rowSet.row_count; i++) {
        for (int j = 0; j < rowSet.rows[i].cell_count; j++) {
            free(rowSet.rows[i].cells[j].value);
        }
        free(rowSet.rows[i].cells);
    }
    free(rowSet.rows);
}

void deleteRow(RowSet *rowSet, int rowIndex) {
    if (rowIndex < 0 || rowIndex >= rowSet->row_count) {
        fprintf(stderr, "Invalid row index\n");
        return;
    }

    Row *row = &rowSet->rows[rowIndex];

    // �ͷŸ��е��ڴ�
    for (int i = 0; i < row->cell_count; i++) {
        free(row->cells[i].value);
    }
    free(row->cells);

    // �����к��������ǰ�ƶ�
    for (int i = rowIndex; i < rowSet->row_count - 1; i++) {
        rowSet->rows[i] = rowSet->rows[i + 1];
    }

    // ��������
    rowSet->row_count--;

    // ���·����ڴ�
    rowSet->rows = (Row *)realloc(rowSet->rows, rowSet->row_count * sizeof(Row));
}

void rowSetToCsv(const char *filename, RowSet rowSet) {
    FILE *file = fopen(filename, "w");
    if (!file) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < rowSet.row_count; i++) {
        Row row = rowSet.rows[i];
        for (int j = 0; j < row.cell_count; j++) {
            Cell cell = row.cells[j];

                fprintf(file, "%s", (char *)cell.value);
            
            if (j < row.cell_count - 1) {
                fprintf(file, ",");
            }
        }
        fprintf(file, "\n");
    }
    fclose(file);
}

int *findindex(RowSet rowSet, const char *condition) {
    // ���������ַ�������ȡ�ȽϷ��ͱȽ�ֵ
    char comparison;
    int value,column;
    if (sscanf(condition, "col%d%c%d",&column, &comparison, &value) != 3) {
        fprintf(stderr, "Failed to parse condition from string.\n");
        return NULL;
    }
    //��ָ����Ԫ�ش���AVL��
    IndexTree indexTree;
    initIndexTree(&indexTree);
    createIndexFromRowSet(&rowSet, &indexTree, column-1);
    // ��ӡ������
    //inOrderTraversal(indexTree.root);
    
    //��ʼ�����ڴ洢�кŵ�����
    int *indexes = new int[rowSet.row_count];
    int indexCount = 0;
    //�������������Ľڵ㣨�߶���ͣ�
    AVLNode node1 = *searchNode(indexTree.root, value);
    //��������
    int *indexess =inOrderTraversal1(&node1,value,rowSet.row_count,indexes,indexCount);
    //��ӡ�к�
    // for(int i=0;indexess[i]!=-1&&i<rowSet.row_count;i++){
    //     printf("%d\n",indexess[i]);
    // }
    return indexess;
}

Row createRowFromCSVString(const char *csvString) {
    Row row;
    row.cells = NULL;
    row.cell_count = 0;

    // �����ַ����Ա����޸�ԭʼ�ַ���
    char *copy = strdup(csvString);
    if (!copy) {
        perror("Memory allocation failed");
        exit(EXIT_FAILURE);
    }

    // ʹ��strtok�ָ��ַ���
    char *token = strtok(copy, ",");
    while (token) {
        // ���ӵ�Ԫ�����
        row.cell_count++;

        // ���·����ڴ��������µĵ�Ԫ��
        row.cells = (Cell *)realloc(row.cells, row.cell_count * sizeof(Cell));
        if (!row.cells) {
            perror("Memory allocation failed");
            exit(EXIT_FAILURE);
        }

        // Ϊ�µ�Ԫ������ڴ沢����ֵ
		strcpy(row.cells[row.cell_count - 1].type, "char*");
        row.cells[row.cell_count - 1].size = strlen(token);
        row.cells[row.cell_count - 1].value = strdup(token);
        if (!row.cells[row.cell_count - 1].value) {
            perror("Memory allocation failed");
            exit(EXIT_FAILURE);
        }

        token = strtok(NULL, ",");
    }

    // �ͷŸ��Ƶ��ַ���
    free(copy);
    return row;
}



// �޸�RowSet��ָ���е�����
void modifyRow(RowSet *rowSet, int rowIndex, Row newRow) {
    if (rowIndex < 0 || rowIndex >= rowSet->row_count) {
        fprintf(stderr, "Invalid row index\n");
        return;
    }

    // �ͷ�ԭ�е��е��ڴ�
    Row *row = &rowSet->rows[rowIndex];
    for (int i = 0; i < row->cell_count; i++) {
        free(row->cells[i].value);
    }
    free(row->cells);

    // ���µ��в��뵽ָ��λ��
    rowSet->rows[rowIndex] = newRow;
}

void printcolumn(RowSet rowSet){
    for (int i = 0; i < rowSet.rows[0].cell_count; i++) {
        printf("|%10s| ",(char *) rowSet.rows[0].cells[i].value); // ��ӡ��Ԫ���ֵ
    }
    printf("\n");
}
void printsinglerow(RowSet rowSet,int value){
    
    for (int i = 0; i < rowSet.rows[value].cell_count; i++) {
        printf("|%10s| ", (char *)rowSet.rows[value].cells[i].value); // ��ӡ��Ԫ���ֵ
    }
    printf("\n");
}

int main(){
	while(1){
        //��ȡָ��
		printf("������ָ��\n");
		char  order[1000];
		cin.getline(order,1000);
		char *mode = strtok(order, " ");
        //����ָ������
		if(strcmp(mode,"CREATE")==0){
			char *extra = strtok(NULL, " ");
            //��������б�Ҫ���޹��ַ���
			if(strcmp(extra,"TABLE")==0){
                //��ȡ�ļ����������ļ�
				char *name = strtok(NULL,"(");
				char filename[100];  
				strcpy(filename, name);
				strcat(filename,".CSV");
				FILE *fp = fopen(filename, "w");
				if (fp == NULL){
               		perror("���ļ�ʧ��");
               		return 1;
           		}
           		char *column = strtok(NULL,")");
				fprintf(fp,"%s",column);
				fclose(fp);
			}
			else{
				perror("�﷨����");
				return 1;
			}
		}
        //����ָ������
		else if(strcmp(mode,"INSERT")==0){
			char *extra = strtok(NULL, " ");
			if(strcmp(extra,"INTO")==0){
                //��ȡ�ļ��������ļ�
				char *name = strtok(NULL,"(");
				char filename[100];  
				strcpy(filename, name);
				strcat(filename,".CSV");
				FILE *fp = fopen(filename, "a");
				if (fp == NULL){
               		perror("���ļ�ʧ��");
               		return 1;
           		}
           		//��ȡ����
				char *column = strtok(NULL,")");
                //������д����ͨ��CSV�ļ����棩
				fprintf(fp,"\n%s",column);         			
				fclose(fp);
			}
			else{
				perror("�﷨����");
				return 1;
			}
		}
		else if(strcmp(mode,"SELECT")==0){
			char *target = strtok(NULL," ");
			if(strcmp(target,"*")==0){
				char *extra = strtok(NULL," ");
				if(strcmp(extra,"FROM")==0){
    				
				}
				else{
					perror("�﷨����");
					return 1;	
				}
			}
			else {
                char *cptarget = _strdup(target);                
                char *extra = strtok(NULL," ");
				if(strcmp(extra,"FROM")==0){
					
				}
				else{
					perror("not from");
					return 1;	
				}
                
            }
            char *name =strtok(NULL," \n");
            //printf("%s",name);
            char *filename =strdup(name);
            strcat(filename,".CSV");
            
            char *extra = strtok(NULL," ");
            //printf("%s\n",extra);
            if(extra==NULL){
                RowSet rowSet = csvToRowSet(filename);
                printRowSet(rowSet);
            }
            else if(strcmp(extra,"WHERE")==0){
                char *demand = strtok(NULL,"\n");
                RowSet rowSet = csvToRowSet(filename);
                int value;
                if(sscanf(demand, "row=%d", &value) == 1){
                    printcolumn(rowSet);
                    printsinglerow(rowSet,value);
                }
                else{
                    int *select_index = findindex(rowSet,demand);
                    printcolumn(rowSet);
                    for(int i=0;select_index[i]!=-1;i++){
                        printsinglerow(rowSet,select_index[i]);
                    }      
                }
            }         
		}
		else if(strcmp(mode,"DELETE")==0){
			char *extra = strtok(NULL," ");
			if(strcmp(extra,"FROM")==0){
				char *name =strtok(NULL," ");
				char filename[100]; 
				strcpy(filename, name);
				strcat(filename,".CSV");
				char *extra = strtok(NULL," ");
				if(strcmp(extra,"WHERE")==0){
					char *demand = strtok(NULL,"\n");
					RowSet rowSet = csvToRowSet(filename);
					int value,delete_index;
                    if(sscanf(demand, "row=%d", &value) == 1){
                        delete_index = value;
                        deleteRow(&rowSet,delete_index);
					    rowSetToCsv(filename, rowSet);
                    }
                    else{
                        int *select_index = findindex(rowSet,demand);
                        for(int i=0;select_index[i]!=-1;i++){
                        deleteRow(&rowSet,select_index[i]-(i));
                        }
                        rowSetToCsv(filename, rowSet);
                    }
					
				}
			}
		}
		else if(strcmp(mode,"UPDATE")==0){
			char *name =strtok(NULL," ");
			char *filename =strdup(name);
			strcat(filename,".CSV");
			char *extra = strtok(NULL," ");
			if(strcmp(extra,"WHERE")==0){
				char *demand = strtok(NULL," ");				
				char *newdemand =strdup(demand);
				char *extra = strtok(NULL,"(");
				if(strcmp(extra,"VALUES")==0){
					char *newdata = strtok(NULL,")");
					Row row = createRowFromCSVString(newdata);
					RowSet rowSet = csvToRowSet(filename);
					int value,update_index;
                    if(sscanf(demand, "row=%d", &value) == 1){
                        update_index = value;
                        modifyRow(&rowSet,update_index, row);
					    rowSetToCsv(filename, rowSet);
                    }
                    else{
                        int *select_index = findindex(rowSet,demand);
                        
                        for(int i=0;select_index[i]!=-1;i++){
                        modifyRow(&rowSet,select_index[i], row);
                        }
                        rowSetToCsv(filename, rowSet);
                    }
                    //printf("%d",update_index);
					
				}
				else {
					perror("not values");
					return 1;
				}
			}
			else {
				perror("not where");
				return 1;
			}
		}else if(strcmp(mode,"EXIT")==0){
			break;
		}
        else {
            perror("�﷨����");
            return 1;
        }

	}

	return 0;
}













