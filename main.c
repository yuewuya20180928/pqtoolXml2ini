/*************************************************************************
	> File Name: main.c
	> Author: 
	> Mail: 
	> Created Time: Mon 10 Oct 2016 05:01:35 PM CST
 ************************************************************************/

#include<stdio.h>
#include<mxml.h>
#include"main.h"
#include <string.h>

mxml_type_t type_cb(mxml_node_t *node)
{
	const char *type;
	/*
	* 你可以查看属性和/或使用XML元素名，所在层次，等等
	*/
	#if 0    
	mxml_attr_t	*elementAttrs;
	elementAttrs = node->value.element.attrs;
	if(NULL != elementAttrs)
		printf("%s\n",elementAttrs->value);
	#endif
	//printf("Name:%s \n",mxmlElementGetAttr(node,"Name")); 
	
	type = mxmlElementGetAttr(node, "type");
	if (type == NULL)
		type = node->value.element.name;
	
	if (!strcmp(type, "integer"))
		return (MXML_INTEGER);
	else if (!strcmp(type, "opaque"))
		return (MXML_OPAQUE);
	else if (!strcmp(type, "real"))
		return (MXML_REAL);
	else
		return (MXML_TEXT);
}

void GetTitle(const char *pSource, char *pTitle, char *pData)
{
	unsigned int i = 0;

	if (pSource == NULL || pTitle == NULL || pData == NULL)
	{
		return;
	}

	for (i = 0; i < strlen(pSource); i++)
	{
		if (pSource[i] == '.')
		{
			break;
		}
	}

	strncpy(pTitle, pSource, i);

	pTitle[i] = '\0';

	strcpy(pData, pSource + i + 1);

	return;
}

int main()
{
	FILE *fp = NULL;
	mxml_node_t *tree;
	mxml_node_t *node;
	mxml_node_t *nodenext;    
	mxml_node_t *nodedata;
	int fieldReadMark = 0;
	int dataReadMark = 0;
	char* pNodeValue = NULL;
	char buffer[35000] = {0};
	char target[35000] = {0};
	char lastTitle[1280] = {0};
	char newOpt[1280] = {0};
	char newTitle[1280] = {0};
	FILE * pFileOut = NULL;
	char* fileNameOut = "target.ini";
	int ret = 0;

	pFileOut = fopen(fileNameOut,"w");
	if (pFileOut == NULL)
	{
		printf("open file:%s error!\n", fileNameOut);
		return -1;
	}
	
	fp = fopen("source.xml", "r");
	if (fp == NULL)
	{
		printf("Target.xml open failed,%s!\n",strerror(errno));
		printf("Please copy *.sav flie to this folder rename as target.xml and try again.\n");
		return 0;
	}

	tree = mxmlLoadFile(NULL, fp, type_cb);
	fclose(fp);
	
	node = tree;
	
	while(1)
	{
		//find title of each page
		node = mxmlFindElement(node, tree, "SAVE_DATAS",NULL, NULL,MXML_DESCEND);  

		if (node)
		{
			nodenext = node;
			nodedata = node;

			while(1)
			{                      
				//find date in <Field></Field>
				nodenext = mxmlFindElement(nodenext, node, "SAVE_DATA",NULL, NULL,MXML_DESCEND);
	
				if (nodenext)
				{
					snprintf(buffer, 1280, "%s", mxmlElementGetAttr(nodenext,"PATH"));

					GetTitle(buffer, newTitle, newOpt);

					if (strcmp(newTitle, lastTitle) != 0)
					{
						strcpy(lastTitle, newTitle);
						snprintf(buffer, 35000, "[%s]\n", lastTitle);
						if (fputs(buffer, pFileOut) == EOF)
						{
							printf("write title error!\n");
							break;
						}
					}

					sprintf(buffer,"%s = \"%s\"\n", newOpt, mxmlElementGetAttr(nodenext, "VALUE")); 
					if (fputs(buffer, pFileOut) == EOF)
					{
						printf("fputs() break in %s\n",buffer);
						break;
					}
				}
				else
				{
					fieldReadMark = 1;
				}
				
				if (1 == fieldReadMark)
				{
					fieldReadMark = 0;
					break;
				}
			}            
		}
		else
		{
			printf("not founded!\n");
			break;
		}
	}
    

#if 0
	nodenext = node->child;        
	printf("nodenext = %d\n",nodenext);        
	printf("Name1:%s \n",mxmlElementGetAttr(node,"Name"));
	
	nodenext = nodenext->next;        
	printf("nodenext = %d\n",nodenext);        
	printf("Name2:%s \n",mxmlElementGetAttr(node,"Name"));
	
	node = tree->child;
	printf("node = %d",node);
	node = node->next;     
	printf("node = %d",node);   
	node = node->child;     
	printf("node = %d",node);
	printf("Movedone\n");
#endif
	fclose(pFileOut);
	mxmlDelete(tree);
	        
	printf(".sav to ini done\n");
	
	//sample_test();
	
	return 0;
}
