/*
 * enux-installer.c -- GUI installer for ENux
 *
 * Depends on libui-ng (checkout https://github.com/libui-ng/libui-ng), build with make.
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <ui.h>

typedef struct App {
	uiWindow*      win;
	uiTab*         tab;
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
} App;

static void
onSwapToggle(uiCheckbox* c, void* data)
{
	App* app = data;
	int checked = uiCheckboxChecked(app->enableSwap);
	if (checked)
		uiControlEnable(uiControl(app->swapSize));
	else
		uiControlDisable(uiControl(app->swapSize));
}

static int
onClosing(uiWindow* w, void* data)
{
	(void)w;
	App* app = data;

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
updateProg(void* data)
{
	App* app = data;
	if (!app->installing)
		return;
	if (app->progress == NULL) return;
	static int value = 0;
	value += 10;

	if (value > 100)
		value = 100;

	uiProgressBarSetValue(app->progress, value);
	char buf[128];
	snprintf(buf, sizeof(buf), "Installing... %d%%", value);
	uiLabelSetText(app->status, buf);

	if (value >= 100) {
		if (!app->installing)
			return;

		app->installing = 0;
		uiMsgBox(app->win,
			"Finished.",
			"Installation has successfully finished!"
		);
	}
}

static void
onInstallClicked(uiButton* b, void* data)
{
	(void)b; /* we know this is the install button */
	App* app = data;

	const char* user = uiEntryText(app->userEnt);
	const char* disk = uiComboboxSelected(app->diskSelect) >= 0
		? "Selected disk"
		: "No disk selected";

	printf("Installing for user %s on %s\n", user, disk);
	app->installing = 1;

	for (int i = 0; i <= 10; i++) {
		uiQueueMain(updateProg, app);
	}
}

static uiControl*
makeWelcomePage(void)
{
	uiForm* f = uiNewForm();
	uiFormSetPadded(f, 1);

	uiFormAppend(f,
		"",
		uiControl(uiNewLabel("Welcome to the ENux installer!")),
		0
	);

	uiFormAppend(f,
		"",
		uiControl(uiNewLabel(
			"This setup will help you set up a working ENux system on your machine"
		)),
		0
	);

	return uiControl(f);
}

static uiControl*
makeDiskPage(App* app)
{
	uiForm* f = uiNewForm();
	uiFormSetPadded(f, 1);

	app->diskSelect = uiNewCombobox();
	uiComboboxAppend(app->diskSelect, "/dev/sda (512GiB SSD)");
	uiComboboxAppend(app->diskSelect, "/dev/nvme0n1 (1TiB NVMe)");
	uiComboboxAppend(app->diskSelect, "/dev/sda (2GB Extractible medium)");

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

	return uiControl(f);
}

static uiControl*
makeUserPage(App* app)
{
	uiForm* f = uiNewForm();
	uiFormSetPadded(f, 1);

	app->userEnt = uiNewEntry();
	app->passEnt = uiNewPasswordEntry();
	app->rootPassEnt = uiNewPasswordEntry();

	uiFormAppend(f,
		"Username",
		uiControl(app->userEnt),
		0
	);

	uiFormAppend(f,
		"Password",
		uiControl(app->passEnt),
		0
	);

	uiFormAppend(f,
		"Root password",
		uiControl(app->rootPassEnt),
		0
	);

	return uiControl(f);
}

static uiControl*
makeInstallPage(App* app)
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

/******************************************************************************
 **                                  main                                    **
 *****************************************************************************/

int
main(int argc, char** argv)
{
	uiInitOptions o = { 0 };
	const char* err = uiInit(&o);

	if (err != NULL) {
		fprintf(stderr, "init errror (libui): %s\n", err);
		uiFreeInitError(err);
		return EXIT_FAILURE;
	}

	App app;
	memset(&app, 0, sizeof(app));

	app.win = uiNewWindow("ENux Installer", 600, 400, 1);
	uiWindowOnClosing(app.win, onClosing, &app);

	app.tab = uiNewTab();

	/* pages */
	uiTabAppend(app.tab, "Welcome",
		makeWelcomePage());

	uiTabAppend(app.tab, "Disk",
		makeDiskPage(&app));

	uiTabAppend(app.tab, "User",
		makeUserPage(&app));
	
	uiTabAppend(app.tab, "Install",
		makeInstallPage(&app));

	uiWindowSetChild(app.win, uiControl(app.tab));
	uiWindowSetMargined(app.win, 1);

	uiControlShow(uiControl(app.win));

	uiMain();
	uiUninit();
	return EXIT_SUCCESS;
}

