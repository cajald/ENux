/*
 * gui.h -- GUI function access
 */

#ifndef GUI_H
#define GUI_H

#include <ui.h>

typedef struct GUI {
	uiWindow*      win;
	uiTab*         tab;
	uiBox*         vbox;
	uiButton*      nextBtn;
	uiButton*      backBtn;
	uiEntry*       userEnt;
	uiEntry*       passEnt;
	uiEntry*       rootPassEnt;
	uiCombobox*    diskSelect;
	uiCheckbox*    separateHome;
	uiCheckbox*    enableSwap;
	uiSpinbox*     swapSize;
	uiCombobox*    fsType;
	uiProgressBar* progress;
	uiLabel*       status;
	int            installing;
	int            progValue;
	int            page;
	uiControl*     pages[4];
} GUI;

GUI* setupUI(void);
void runUI(void);
void teardownUI(GUI* app);

#endif /* GUI_H */

