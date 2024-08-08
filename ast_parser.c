#include <stdio.h>
#include <string.h>
#include "json_c.c"

int ifNum= 0;

void countIf(json_value block){
    // 3가지 경우 iffalse, iftrue, while문
    if(strcmp(json_get_string(block, "_nodetype"),"If") == 0){
        ifNum++;

        json_value ifFalse = json_get(block, "iffalse");
        if(json_len(ifFalse) != 0){
            countIf(ifFalse);
            json_value falseBlockItems = json_get(ifFalse, "block_items");
            if(json_len(falseBlockItems) != 0){
                for(int i=0; i<json_len(falseBlockItems); ++i){
                    json_value falseBlock = json_get(falseBlockItems, i);
                    countIf(falseBlock);
                }
            }
        }

        json_value ifTrue = json_get(block, "iftrue");
         if(json_len(ifTrue) != 0){
            countIf(ifTrue);
            json_value trueBlockItems = json_get(ifTrue, "block_items");
            if(json_len(trueBlockItems) != 0){
                for(int i=0; i<json_len(trueBlockItems); ++i){
                    json_value trueBlock = json_get(trueBlockItems, i);
                    countIf(trueBlock);
                }
            }
        }
    }

    else if(strcmp(json_get_string(block, "_nodetype"), "While") == 0){
        json_value stmt = json_get(block, "stmt");
        json_value stmtBlockItems = json_get(stmt, "block_items");

        if(json_len(stmtBlockItems) != 0){
            for(int i=0; i<json_len(stmtBlockItems); ++i){
                json_value stmtBlock = json_get(stmtBlockItems, i);
                countIf(stmtBlock);
            }

        }
    }

}

int main(void){
    FILE *f;

    int fnNum = 0;

    f = fopen("target.json", "r");

    if(f==NULL){
        printf("file fail\n");
    }
    else{
        printf("file success\n");
    }

    int size;
    char *buffer;

    // 파일 크기 구하기
    fseek(f, 0, SEEK_END);
    size = ftell(f);
    fseek(f, 0, SEEK_SET);

    // 파일 크기 + NULL 공간만큼 메모리를 할당하고 0으로 초기화
    buffer = malloc(size + 1);
    memset(buffer, 0, size + 1);

    // 파일 내용 읽기
    fread(buffer, size, 1, f);
    
    // json 파일로 변환
    json_value json = json_create(buffer);  

    free(buffer);
    fclose(f);
 
    json_value ext = json_get(json, "ext");
    int ext_size = json_len(ext);

    // json_print(json_get(ext, 0));
    for(int i=0; i<ext_size; ++i){
        json_value obj = json_get(ext, i);

        char *nodeType = json_get_string(obj, "_nodetype"); // 타입

        if(strcmp(nodeType, "FuncDef") == 0){
            fnNum++;
            // 함수 이름
            json_value decl = json_get(obj, "decl");
            char *fnName = json_get_string(decl, "name");

            printf("함수 이름 : %s\n", fnName);

            // 리턴 타입
            json_value type1 = json_get(decl, "type");
            json_value type2 = json_get(type1, "type");
            json_value type3 = json_get(type2, "type");

            if(strcmp(json_get_string(type2, "_nodetype"), "PtrDecl") == 0){
                type3 = json_get(type3, "type");
            }
            

            json_value typeName = json_get(type3, "names");

            char *retType = json_get_string(json_get(typeName, 0));

            printf("리턴 타입 : %s\n", retType);

            //파라미터 타입, 변수명
            json_value args = json_get(type1, "args");

            if(!json_is_null(args)){
                json_value params = json_get(args, "params");
                int params_size = json_len(params);

                for(int j=0; j<params_size; ++j){
                    // 파라미터 변수명
                    json_value param = json_get(params, j);
                    char *paramName = json_get_string(param, "name");

                    printf("파라미터 이름 : %s, ", paramName);

                    // 파라미터 타입
                    printf("파라미터 타입 : ");
                    json_value paramType1 = json_get(param, "type");
                    json_value paramType2 = json_get(paramType1, "type");

                    // 포인터 변수일땐 한번 더 들어가기
                    if(strcmp(json_get_string(paramType1, "_nodetype"), "PtrDecl") == 0){
                        paramType2 = json_get(paramType2, "type");
                        printf("pointer ");
                    }

                    json_value paramTypeName = json_get(paramType2, "names");
                    char *paramType = json_get_string(json_get(paramTypeName, 0));

                    printf("%s\n", paramType);
                }
            }

            // 함수들의 if 조건 갯수
            json_value body = json_get(obj, "body");
            json_value block_items = json_get(body, "block_items");

            ifNum = 0;

            int block_items_size = json_len(block_items);
            for(int k=0; k<block_items_size; ++k){
                json_value block = json_get(block_items, k);
                countIf(block);
            }

            printf("if 조건 갯수 : %d\n", ifNum);
            


            printf("\n");
        }   
    }

    printf("함수 갯수 : %d", fnNum);

    return 0;
}