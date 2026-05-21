/*
 * gui.c -- GUI installer for ENux -- UI
 */

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include <ui.h>

#include "gui.h"
#include "part.h"

/******************************************************************************
 **                                 helpers                                  **
 *****************************************************************************/

void
updateNav(GUI* app)
{
	uiControlEnable(uiControl(app->backBtn));
	uiControlEnable(uiControl(app->nextBtn));
	if (!app->allowNext)
		uiControlDisable(uiControl(app->nextBtn));
}

static void
updateProg(void* data)
{
	GUI* app = data;
	if (!app->installing)
		return;
	if (app->progress == NULL) return;
	app->progValue += 10;
 
	if (app->progValue > 100)
		app->progValue = 100;

	uiProgressBarSetValue(app->progress, app->progValue);
	char buf[128];
	snprintf(buf, sizeof(buf), "Installing... %d%%", app->progValue);
	uiLabelSetText(app->status, buf);

	if (app->progValue >= 100) {
		if (!app->installing)
			return;

		app->installing = 0;
		uiMsgBox(app->win,
			"Finished.",
			"Installation has successfully finished!"
		);
	}
}

/******************************************************************************
 **                                handlers                                  **
 *****************************************************************************/

static void
onPartClicked(uiButton* b, void* data)
{
	(void)b; (void)data;

	pid_t pid = fork();

	if (pid < 0) {
		/* fork failure */
		perror("fork error");
		exit(EXIT_FAILURE);
	} else if (pid == 0) {
		/* child */
		execlp("gparted", "gparted", NULL);
		_exit(1);
	}
}

static int
onClosing(uiWindow* w, void* data)
{
	(void)w;
	GUI* app = data;

	if (app->installing) {
		uiMsgBoxError(app->win,
			"Installer is running!",
			"Please wait untill installation finishes."
		);
		return 0;
	}

	uiQuit();
	return 1;
}

static void
onQuitClicked(uiButton* b, void* data)
{
	(void)b;

	GUI* app = data;

	if (app->installing) {
		uiMsgBoxError(app->win,
			"Installer is running!",
			"Please wait until installation finishes."
		);
		return;
	}

	uiControlDestroy(uiControl(app->win));
	uiQuit();
}

static void
onTabChanged(uiTab* t, void* data)
{
	GUI* app = data;

	if (uiTabSelected(t) != app->page)
		uiTabSetSelected(t, app->page);
}

static void
onNextClicked(uiButton* b, void* data)
{
	GUI* app = data;

	if (app->nextCb)
		app->nextCb(app);

	if (!app->allowNext)
		return;

	if (app->page < 3)
		app->page++;

	uiTabSetSelected(app->tab, app->page);
	updateNav(app);
}

static void
onBackClicked(uiButton* b, void* data)
{
	GUI* app = data;

	if (app->page > 0)
		app->page--;

	uiTabSetSelected(app->tab, app->page);
	updateNav(app);
}

static void
onSwapToggle(uiCheckbox* c, void* data)
{
	GUI* app = data;
	int checked = uiCheckboxChecked(app->enableSwap);
	if (checked)
		uiControlEnable(uiControl(app->swapSize));
	else
		uiControlDisable(uiControl(app->swapSize));

	validateAll(app);
}

static void
onInstallClicked(uiButton* b, void* data)
{
	(void)b; /* we know this is the install button */
	GUI* app = data;

	const char* user = uiEntryText(app->userEnt);
	const char* disk = uiComboboxSelected(app->diskSelect) >= 0
		? "Selected disk"
		: "No disk selected";

	printf("Installing for user %s with %s\n", user, disk);
	app->installing = 1;

	for (int i = 0; i <= 10; i++) {
		uiQueueMain(updateProg, app);
	}
}

/******************************************************************************
 **                                 content                                  **
 *****************************************************************************/

static uiControl*
makeWelcomePage(void)
{
	uiBox* vbox = uiNewVerticalBox();
	uiBoxSetPadded(vbox, 1);

	/* centered title using spacers */
	uiBox* titleRow = uiNewHorizontalBox();

	uiLabel* leftSpacer = uiNewLabel("");
	uiLabel* title = uiNewLabel("ENUX OPERATING SYSTEM");
	uiLabel* rightSpacer = uiNewLabel("");

	uiBoxAppend(titleRow, uiControl(leftSpacer), 1);
	uiBoxAppend(titleRow, uiControl(title), 0);
	uiBoxAppend(titleRow, uiControl(rightSpacer), 1);

	/* subtitle */
	uiLabel* subtitle = uiNewLabel(
		"This wizard will guide you through the\n"
		"installation of ENux on your computer."
	);

	/* info group */
	uiGroup* info = uiNewGroup("Before you begin");
	uiBox* infoBox = uiNewVerticalBox();
	uiBoxSetPadded(infoBox, 1);

	uiBoxAppend(infoBox,
		uiControl(uiNewLabel(
			"	• Make sure you selected the correct disk\n"
			"	• Back up important data\n"
			"	• Plug in your device during installation"
		)),
		0
	);

	uiGroupSetChild(info, uiControl(infoBox));

	/* layout */
	uiBoxAppend(vbox, uiControl(titleRow), 0);
	uiBoxAppend(vbox, uiControl(uiNewHorizontalSeparator()), 0);
	uiBoxAppend(vbox, uiControl(subtitle), 0);
	uiBoxAppend(vbox, uiControl(info), 0);

	return uiControl(vbox);
}

static uiControl*
makeDiskPage(GUI* app)
{
	uiForm* f = uiNewForm();
	uiFormSetPadded(f, 1);

	app->diskSelect = uiNewCombobox();
	size_t n = 0;
	Part* parts = getparts(&n);

	if (!parts) {
		uiComboboxAppend(app->diskSelect, "<No disks found>");
	} else {
		for (size_t i = 0; i < n; i++) {
			if (parts[i].minor != 0)
				continue;
			char label[128];

			double gib = parts[i].blocks / (1024.0 * 1024.0);

			snprintf(
				label,
				sizeof(label),
				"/dev/%s (%.1f GiB)",
				parts[i].name,
				gib
			);

			uiComboboxAppend(app->diskSelect, label);
		}

		free(parts);
	}

	uiFormAppend(f,
		"Select install disk",
		uiControl(app->diskSelect),
		0
	);

	app->fsType = uiNewCombobox();
	uiComboboxAppend(app->fsType, "ext4");
	uiComboboxAppend(app->fsType, "btrfs");
	uiComboboxAppend(app->fsType, "zfs");

	uiFormAppend(f,
		"Select filesystem type",
		uiControl(app->fsType),
		0
	);

	app->separateHome = uiNewCheckbox("Separate /home partition");

	uiFormAppend(f,
		"Separate home",
		uiControl(app->separateHome),
		0
	);

	app->enableSwap = uiNewCheckbox("Enable swap");
	uiCheckboxOnToggled(app->enableSwap, onSwapToggle, app);

	uiFormAppend(f,
		"Enable swap",
		uiControl(app->enableSwap),
		0
	);

	app->swapSize = uiNewSpinbox(0, INT32_MAX);
	uiSpinboxSetValue(app->swapSize, 1024);
	uiControlDisable(uiControl(app->swapSize)); /* start disabled */

	uiFormAppend(f,
		"Swap size (in MB)",
		uiControl(app->swapSize),
		0
	);

	app->partBtn = uiNewButton("Open gparted");
	uiButtonOnClicked(app->partBtn, onPartClicked, app);

	uiFormAppend(f,
		"Partition editor",
		uiControl(app->partBtn),
		0
	);

	return uiControl(f);
}

static uiControl*
makeUserPage(GUI* app)
{
	uiForm* f = uiNewForm();
	uiFormSetPadded(f, 1);

	app->userEnt = uiNewEntry();
	app->userRealNameEnt = uiNewEntry();
	app->passEnt = uiNewPasswordEntry();
	app->repPassEnt = uiNewPasswordEntry();
	app->rootPassEnt = uiNewPasswordEntry();
	app->repRootPassEnt = uiNewPasswordEntry();

	uiFormAppend(f,
		"Short Username",
		uiControl(app->userEnt),
		0
	);

	uiFormAppend(f,
		"Real Name",
		uiControl(app->userRealNameEnt),
		0
	);

	uiFormAppend(f,
		"Password",
		uiControl(app->passEnt),
		0
	);

	uiFormAppend(f,
		"Repeat password",
		uiControl(app->repPassEnt),
		0
	);

	uiFormAppend(f,
		"Root password",
		uiControl(app->rootPassEnt),
		0
	);

	uiFormAppend(f,
		"Repeat root password",
		uiControl(app->repRootPassEnt),
		0
	);

	return uiControl(f);
}

static uiControl*
makeInstallPage(GUI* app)
{
	uiBox* vbox = uiNewVerticalBox();
	uiBoxSetPadded(vbox, 1);

	/* status */
	app->status = uiNewLabel("Ready to install.");
	uiGroup* statusGroup = uiNewGroup("Status");
	uiGroupSetChild(statusGroup, uiControl(app->status));

	uiBoxAppend(uiBox(vbox),
		uiControl(statusGroup),
		0
	);

	/* progress */
	app->progress = uiNewProgressBar();

	uiGroup* progressGroup = uiNewGroup("Progress");
	uiGroupSetChild(progressGroup, uiControl(app->progress));

	uiBoxAppend(uiBox(vbox),
		uiControl(progressGroup),
		0
	);

	uiButton* btn = uiNewButton("Install now!");
	uiButtonOnClicked(btn, onInstallClicked, app);

	uiBoxAppend(uiBox(vbox),
		uiControl(uiNewHorizontalSeparator()),
		0
	);
	uiBoxAppend(uiBox(vbox),
		uiControl(btn),
		0
	);

	return uiControl(vbox);
}

static uiControl*
makeNavBar(GUI* app)
{
	uiBox* h = uiNewHorizontalBox();
	uiBoxSetPadded(h, 1);

	app->quitBtn = uiNewButton("Quit");
	app->backBtn = uiNewButton("< Back");
	app->nextBtn = uiNewButton("Next >");

	uiButtonOnClicked(app->backBtn, onBackClicked, app);
	uiButtonOnClicked(app->nextBtn, onNextClicked, app);
	uiButtonOnClicked(app->quitBtn, onQuitClicked, app);

	/* align buttons right */
	uiLabel* spacer = uiNewLabel("");
	uiLabel* spacer2 = uiNewLabel("");

	uiBoxAppend(h, uiControl(app->quitBtn), 0);
	uiBoxAppend(h, uiControl(spacer), 1);
	if (!app->allowNext)
		uiBoxAppend(h, uiControl(uiNewLabel("Invalid fields")), 0);
	uiBoxAppend(h, uiControl(spacer2), 1);
	uiBoxAppend(h, uiControl(app->backBtn), 0);
	uiBoxAppend(h, uiControl(app->nextBtn), 0);

	updateNav(app);

	return uiControl(h);
}

/******************************************************************************
 **                                  main                                    **
 *****************************************************************************/

GUI*
setupUI(void (*nextCb)(GUI*))
{
	uiInitOptions o = { 0 };
	const char* err = uiInit(&o);

	if (err != NULL) {
		fprintf(stderr, "init errror (libui): %s\n", err);
		uiFreeInitError(err);
		return NULL;
	}

	GUI* app = (GUI*)calloc(1, sizeof(GUI));
	app->nextCb = nextCb;

	app->win = uiNewWindow("ENux Installer", 600, 400, 1);
	uiWindowOnClosing(app->win, onClosing, app);

	app->vbox = uiNewVerticalBox();
	uiBoxSetPadded(app->vbox, 1);

	/* pages */
	app->pages[0]  = makeWelcomePage();
	app->pages[1]  = makeDiskPage(app);
	app->pages[2]  = makeUserPage(app);
	app->pages[3]  = makeInstallPage(app);
	app->allowNext = true;

	app->tab = uiNewTab();

	uiEntryOnChanged(app->userEnt, onEntryChanged, app);
	uiEntryOnChanged(app->passEnt, onEntryChanged, app);
	uiEntryOnChanged(app->repPassEnt, onEntryChanged, app);
	uiComboboxOnSelected(app->fsType, onComboChanged, app);

	uiCheckboxOnToggled(app->enableSwap, onCheckToggled, app);
	uiSpinboxOnChanged(app->swapSize, onSpinChanged, app);

	uiTabAppend(app->tab, "Welcome", app->pages[0]);
	uiTabAppend(app->tab, "Disk", app->pages[1]);
	uiTabAppend(app->tab, "User", app->pages[2]);
	uiTabAppend(app->tab, "Install", app->pages[3]);
	uiTabOnSelected(app->tab, onTabChanged, app);

	uiBoxAppend(app->vbox, uiControl(app->tab), 1);
	uiBoxAppend(app->vbox, makeNavBar(app), 0);

	uiWindowSetChild(app->win, uiControl(app->vbox));
	uiWindowSetMargined(app->win, 1);

	uiControlShow(uiControl(app->win));

	return app;
}

void
runUI(void)
{
	uiMain();
}

void
blockNext(GUI* app)
{
	app->allowNext = false;
	updateNav(app);
}

void
teardownUI(GUI* app)
{
	free(app);
	uiUninit();
}

