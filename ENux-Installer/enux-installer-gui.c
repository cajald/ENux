/*
 * enux-installer.c -- GUI installer for ENux
 *
 * Depends on libui-ng (checkout https://github.com/libui-ng/libui-ng), build with make.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <ui.h>

typedef struct App {
	uiWindow*      win;
	uiTab*         tab;
	uiEntry*       userEnt;
	uiEntry*       passEnt;
	uiCombobox*    diskSelect;
	uiProgressBar* progress;
	uiLabel*       status;
	int            installing;
} App;

int
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
	if (app->progress == NULL) return;
	static int value = 0;
	value += 10;

	if (value > 100)
		value = 100;

	uiProgressBarSetValue(app->progress, value);
	char buf[128];
	snprintf(buf, sizeof(buf), "Installing... %d%%", value);
	uiLabelSetText(app->status, buf);

	if (value == 100) {
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
	uiBox* v = uiNewVerticalBox();
	uiBoxSetPadded(v, 1);

	uiBoxAppend(v,
		uiControl(uiNewLabel("Welciome to the ENux installer!")),
		0
	);

	uiBoxAppend(v,
		uiControl(uiNewLabel("This setup will help you set up a working ENUX system "
		                     "on your machine")),
		0
	);

	return uiControl(v);
}

static uiControl*
makeDiskPage(App* app)
{
	uiBox* v = uiNewVerticalBox();
	uiBoxSetPadded(v, 1);

	app->diskSelect = uiNewCombobox();
	uiComboboxAppend(app->diskSelect, "/dev/sda (512GiB SSD)");
	uiComboboxAppend(app->diskSelect, "/dev/nvme0n1 (1TiB NVMe)");
	uiComboboxAppend(app->diskSelect, "/dev/sda (2GB Extractible medium)");

	uiBoxAppend(v, uiControl(uiNewLabel("Select install disk:")), 0);
	uiBoxAppend(v, uiControl(app->diskSelect), 0);

	return uiControl(v);
}

static uiControl*
makeUserPage(App* app)
{
	uiBox* v = uiNewVerticalBox();
	uiBoxSetPadded(v, 1);
	
	app->userEnt = uiNewEntry();
	app->passEnt = uiNewPasswordEntry();

	uiBoxAppend(v, uiControl(uiNewLabel("Create an user account")), 0);
	uiBoxAppend(v, uiControl(app->userEnt), 0);
	uiBoxAppend(v, uiControl(app->passEnt), 0);

	return uiControl(v);
}

static uiControl*
makeInstallPage(App* app)
{
	uiBox* v = uiNewVerticalBox();
	uiBoxSetPadded(v, 1);

	app->progress = uiNewProgressBar();
	app->status   = uiNewLabel("Ready to install.");

	uiButton* btn = uiNewButton("Install now!");
	uiButtonOnClicked(btn, onInstallClicked, app);

	uiBoxAppend(v, uiControl(app->status), 0);
	uiBoxAppend(v, uiControl(app->progress), 0);
	uiBoxAppend(v, uiControl(btn), 0);

	return uiControl(v);
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

