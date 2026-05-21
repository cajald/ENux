/*
 * gui.h -- GUI function access
 */

#ifndef GUI_H
#define GUI_H

#include <stdbool.h>

#include <ui.h>

typedef struct GUI {
	uiWindow*      win;
	uiTab*         tab;
	uiBox*         vbox;
	uiButton*      quitBtn;
	uiButton*      nextBtn;
	uiButton*      backBtn;
	uiEntry*       userEnt;
	uiEntry*       passEnt;
	uiEntry*       repPassEnt;
	uiEntry*       repRootPassEnt;
	uiEntry*       userRealNameEnt;
	uiEntry*       rootPassEnt;
	uiCombobox*    diskSelect;
	uiCheckbox*    separateHome;
	uiCheckbox*    enableSwap;
	uiSpinbox*     swapSize;
	uiButton*      partBtn;
	uiCombobox*    fsType;
	uiProgressBar* progress;
	uiLabel*       status;
	void           (*nextCb)(struct GUI* gui);
	int            installing;
	int            progValue;
	int            page;
	uiControl*     pages[4];
	bool           allowNext;
} GUI;

enum {
	PAGE_WELCOME = 0,
	PAGE_DISK, 
	PAGE_USER,
	PAGE_INSTALL,
};

GUI* setupUI(void (*nextCb)(GUI*));
void runUI(void);
void teardownUI(GUI* app);
void updateNav(GUI* app);
void blockNext(GUI* app);

void validateAll(GUI* app);

void onEntryChanged(uiEntry* e, void* data);
void onComboChanged(uiCombobox* c, void* data);
void onSpinChanged(uiSpinbox* s, void* data);
void onCheckToggled(uiCheckbox* c, void* data);

#endif /* GUI_H */

