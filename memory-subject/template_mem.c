
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define NB_DISK_PAGE 5
#define PAGE_SIZE 20
#define NB_MEM_PAGE 4

#define boolean int
#define true 1
#define false 0

char Disk[NB_DISK_PAGE][PAGE_SIZE];		
char Memory[NB_MEM_PAGE][PAGE_SIZE]; 	// RAM
int PageTable[NB_DISK_PAGE];			// locate logical memory to
struct {
	boolean free;
	int date;
	int npage;
} MemState[NB_MEM_PAGE];

int Date = 1;

void init();
char *memory_read(int npage);
int memory_alloc();
void page_fault(int npage);
int lru_select();

void init() {
	int i;

	// initialize Disk
	for (i=0;i<NB_DISK_PAGE;i++) {
		strcpy(Disk[i],"page");
		Disk[i][4] = '0'+i;
		Disk[i][5] = 0;
	}
	/*
	for (i=0;i<NB_DISK_PAGE;i++) {
		printf(Disk[i]);
		printf("\n");
	}
	*/

	// initialize Memory
	for (i=0;i<NB_MEM_PAGE;i++)
		MemState[i].free = true;

	// initialize PageTable
	for (i=0;i<NB_DISK_PAGE;i++)
		PageTable[i] = -1;
}


char *memory_read(int npage) {
	if (PageTable[npage] >= 0 && PageTable[npage] < 4) {
		MemState[PageTable[npage]].date = Date++;
		return "Page already in memory";
	} 
	else if (PageTable[npage] == -1) { // Page havent been loaded from disk yet
		page_fault(npage);
		MemState[PageTable[npage]].date = Date++;
		return "Page have been loaded from disk";
	}
	else {
		printf("Error locating page from memory and disk\n");
		exit(1);
	} 
}

int memory_alloc() {
	for (int i = 0; i < NB_MEM_PAGE; i++) {
		if (MemState[i].free == true) {
			MemState[i].free = false;
			return i;
		}
	}
	return -1;
}

void page_fault(int npage) {
	printf("page_fault (%d)\n",npage);
	// Check if there are any free page
	int location = memory_alloc();
	if (location >= 0 && location < 4) {
		// Loading page to memory (already make a free space)
		strcpy(Memory[location], Disk[npage]);
		PageTable[npage] = location;
		MemState[location].npage = npage;
	}
	else if (location == -1) {
		location = lru_select();
		strcpy(Memory[location], Disk[npage]);
		PageTable[npage] = location;
		MemState[location].npage = npage;
	}
}

int lru_select() {
	int lowest_date = MemState[0].date;
	int lowest_date_index = 0;
	for (int i = 1; i < NB_MEM_PAGE; i++) {
		if (lowest_date > MemState[i].date) {
			lowest_date = MemState[i].date;
			lowest_date_index = i;
		}
	}
	MemState[lowest_date_index].free = false;
	PageTable[MemState[lowest_date_index].npage] = -1;
	printf("Push %d back to disk\n", lowest_date_index);
	return lowest_date_index;
}

int main(int argc, char *argv[]) {
	int i;
	printf("initialization\n\n");
	init();

/*
	printf("linear access of all pages\n");
	for (i=0;i<NB_DISK_PAGE;i++)
		printf("read access page %d : %s\n",i,memory_read(i));
*/

	printf("access pages as in lecture (0,1,2,3,0,1,4,0,1,2,3,4,1,2)\n");
	int serie[14] = {0,1,2,3,0,1,4,0,1,2,3,4,1,2};
	for (i=0;i<14;i++)
		printf("read access page %d : %s\n",serie[i],memory_read(serie[i]));
	printf("completed\n");
	printf("print memory_state\n");
	for (i=0;i<NB_MEM_PAGE;i++)
		printf("%d ",MemState[i].npage);
	printf("\n");
	printf("completed\n");
}


