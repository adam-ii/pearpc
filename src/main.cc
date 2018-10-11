/*
 *	PearPC
 *	main.cc
 *
 *	Copyright (C) 2003-2005 Sebastian Biallas (sb@biallas.net)
 *
 *	This program is free software; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License version 2 as
 *	published by the Free Software Foundation.
 *
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with this program; if not, write to the Free Software
 *	Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <cstdlib>
#include <cstring>
#include <exception>
#include <unistd.h>
#include <thread>
#include <atomic>

#include "info.h"
#include "cpu/cpu.h"
//#include "cpu_generic/ppc_tools.h"
#include "debug/debugger.h"
#include "io/io.h"
#include "io/graphic/gcard.h"
#include "io/ide/ide.h"
#include "io/ide/cd.h"
#include "io/cuda/cuda.h"
#include "io/prom/prom.h"
#include "io/prom/promboot.h"
#include "io/prom/prommem.h"
#include "tools/atom.h"
#include "tools/data.h"
#include "tools/except.h"
#include "tools/snprintf.h"
#include "system/display.h"
#include "system/mouse.h"
#include "system/keyboard.h"
#include "system/sys.h"
#include "clientconfig.h"
#include "main.h"

#include "system/gif.h"
#include "system/ui/gui.h"

#include "ppc_font.h"
#include "ppc_img.h"
#include "ppc_button_changecd.h"

static_assert(sizeof(uint8)  == 1, "sizeof(uint8) != 1");
static_assert(sizeof(uint16) == 2, "sizeof(uint16) != 2");
static_assert(sizeof(uint32) == 4, "sizeof(uint32) != 4");
static_assert(sizeof(uint64) == 8, "sizeof(uint64) != 8");

using namespace pearpc;

void changeCDFunc(void *p)
{
	int *i = (int *)p;
	IDEConfig *idecfg = ide_get_config(*i);
	
	CDROMDevice *dev = (CDROMDevice *)idecfg->device;
	
	dev->acquire();
	
	if (dev->isLocked()) {
		dev->release();
		
		// sys_gui_messagebox("cdrom is locked!");
	} else {
		dev->setReady(false);
		dev->release();
		/*
		 * because we have set ready to false, no one can use
		 * the cdrom now (no medium present)
		 */
		String fn;
		if (sys_gui_open_file_dialog(fn, "title", "*.*", "alle", "testa", true)) {
			dev->acquire();
			((CDROMDeviceFile *)dev)->changeDataSource(fn.contentChar());
			dev->setReady(true);
			dev->release();
		} else {
			/*
			 * the user picked no file / canceled the dialog.
			 * what's better now, to leave the old medium
			 * or to set no medium present?
			 * we choose the second option.
			 */
		}
	}
}

void initMenu()
{
/*	IDEConfig *idecfg = ide_get_config(0);
	if (idecfg->installed && idecfg->protocol == IDE_ATAPI) {
		MemMapFile changeCDButton(ppc_button_changecd, sizeof ppc_button_changecd);
		int *i = new int;
		*i = 0;
		gDisplay->insertMenuButton(changeCDButton, changeCDFunc, i);
	}
	idecfg = ide_get_config(1);
	if (idecfg->installed && idecfg->protocol == IDE_ATAPI) {
		MemMapFile changeCDButton(ppc_button_changecd, sizeof ppc_button_changecd);
		int *i = new int;
		*i = 1;
		gDisplay->insertMenuButton(changeCDButton, changeCDFunc, i);
	}
	gDisplay->finishMenu();*/
}

static const char *textlogo UNUSED = "\e[?7h\e[40m\e[2J\e[40m\n\n\n\n\n\e[0;1m"
"\e[24C\xda\xc4\xc4\xc4\xc4\xc4\xc4\xc4  "
"\xda\xc4\xc4\xc4\xc4\xc4\xc4\xc4   "
"\xda\xc4\xc4\xc4\xc4\xc4\xc4\n\e[24C\e[0m\xda\xc4\xc4   "
"\xda\xc4\xc4 \xda\xc4\xc4   \xda\xc4\xc4 \xda\xc4\xc4   "
"\xda\xc4\xc4\n\e[24C\e[1;30m\xda\xc4\xc4\xc4\xc4\xc4\xc4\xc4  "
"\xda\xc4\xc4\xc4\xc4\xc4\xc4\xc4  "
"\xda\xc4\xc4\n\e[24C\e[34m\xda\xc4\xc4\e[7C\xda\xc4\xc4\e[7C\xda\xc4\xc4   "
"\xda\xc4\xc4\n\e[24C\e[0;34m\xda\xc4\xc4\e[7C\xda\xc4\xc4\e[8C\xda\xc4\xc4\xc4\xc4\xc4\xc4\n\n";

static const vcp CONSOLE_BG = VC_BLACK;

void drawLogo()
{
	MemMapFile img(ppc_img, sizeof ppc_img);
	Gif g;
	g.loadFromByteStream(img);
	gDisplay->fillRGB(0, 0, gDisplay->mClientChar.width,
		gDisplay->mClientChar.height, MK_RGB(0xff, 0xff, 0xff));
	g.draw(gDisplay, (gDisplay->mClientChar.width-g.mWidth)/2, (gDisplay->mClientChar.height >= 600) ? (150-g.mHeight)/2 : 0);
	gDisplay->setAnsiColor(VCP(VC_BLUE, CONSOLE_BG));
	gDisplay->fillAllVT(VCP(VC_BLUE, CONSOLE_BG), ' ');
//	gDisplay->print(textlogo);
	gDisplay->setAnsiColor(VCP(VC_LIGHT(VC_BLUE), VC_TRANSPARENT));
	gDisplay->print("\e[H" APPNAME " " APPVERSION " " COPYRIGHT"\n\n");
}

void tests()
{
/*	while (true) {
		DisplayEvent ev;
		if (gDisplay->getEvent(ev)) {
			if (ev.type == evMouse) {
				gDisplay->printf("%x, %x  ", ev.mouseEvent.relx, ev.mouseEvent.rely);
				gDisplay->printf("%x\n", ev.mouseEvent.button1);
			} else {
				gDisplay->printf("%x %d\n", ev.keyEvent.keycode, ev.keyEvent.keycode);
			}
		}
	}*/
}

#include "io/prom/forth.h"
void testforth()
{

#if 0
		ForthVM vm;
		gCPU.msr = MSR_IR | MSR_DR | MSR_FP;
//		LocalFile in("test/test.f2", IOAM_READ, FOM_EXISTS);
//		vm.interprete(in, in);
		do {
			try {
				MemoryFile in(0);
				char buf[1024];
				fgets(buf, sizeof buf, stdin);
				in.write(buf, strlen(buf));
				in.seek(0);
				vm.interprete(in, in);
			} catch (const ForthException &fe) {
				String res;
				fe.reason(res);
				ht_printf("exception: %y\n", &res);
			}
		} while (1);

#endif
}

/*
 *
 */
void usage() 
{
	ht_printf("usage: ppc configfile\n");
	exit(1);
}

#ifdef main
// Get rid of stupid SDL main redefinitions
#undef main
extern "C" int SDL_main(int argc, char *argv[])
{
	return 0;
}
#endif

namespace pearpc {
	namespace main {
		
		std::shared_ptr<ClientConfig> loadConfig(const char *filename)
		{
			auto clientConfig = std::make_shared<ClientConfig>();
			
			prom_init_config();
			io_init_config();
			ppc_cpu_init_config();
			debugger_init_config();
			
			clientConfig->Load(filename);
			return clientConfig;
		}

		void initClient(const std::shared_ptr<ClientConfig>& clientConfig, const std::function<void (const char*)>& funcInitUI)
		{
			gcard_init_modes();
			gcard_add_characteristic(clientConfig->getDisplayConfig());
			
			/*
			 *	begin hardware init
			 */
			
			if (!ppc_init_physical_memory(clientConfig->getMemorySize())) {
				throw MsgException("cannot initialize memory.");
			}
			if (!ppc_cpu_init()) {
				throw MsgException("cpu_init failed! Out of memory?");
			}
			
			cuda_pre_init();

			if (funcInitUI != nullptr) {
				funcInitUI(APPNAME " " APPVERSION);
			}
			
			io_init();
			
			gcard_init_host_modes();
			gcard_set_mode(clientConfig->getDisplayConfig());
			
			if (clientConfig->getFullScreen()) gDisplay->setFullscreenMode(true);
			
			MemMapFile font(ppc_font, sizeof ppc_font);
			// FIXME: ..
			if (gDisplay->mClientChar.height >= 600) {
				int width = (gDisplay->mClientChar.width-40)/8;
				int height = (gDisplay->mClientChar.height-170)/15;
				if (!gDisplay->openVT(width, height, (gDisplay->mClientChar.width-width*8)/2, 150, font)) {
					throw MsgException("Can't open virtual terminal.");
				}
			} else {
				if (!gDisplay->openVT(77, 25, 12, 100, font)) {
					throw MsgException("Can't open virtual terminal.");
				}
			}
			
			initMenu();
			drawLogo();
			
			// now gDisplay->printf works
			gDisplay->printf("CPU: PVR=%08x\n", ppc_cpu_get_pvr(0));
			gDisplay->printf("%d MiB RAM\n", ppc_get_memory_size() / (1024*1024));
			
			tests();
			
			// initialize initial paging (for prom)
			uint32 PAGE_TABLE_ADDR = gConfig->getConfigInt("page_table_pa");
			gDisplay->printf("initializing initial page table at %08x\n", PAGE_TABLE_ADDR);
			
			// 256 Kbytes Pagetable, 2^15 Pages, 2^12 PTEGs
			if (!ppc_prom_set_sdr1(PAGE_TABLE_ADDR+0x03, false)) {
				throw MsgException("internal error setting sdr1.");
			}
			
			// clear pagetable
			if (!ppc_dma_set(PAGE_TABLE_ADDR, 0, 256*1024)) {
				throw MsgException("cannot access page table.");
			}
			
			// init prom
			prom_init();
			
			// lock pagetable
			for (uint32 pa = PAGE_TABLE_ADDR; pa < (PAGE_TABLE_ADDR + 256*1024); pa += 4096) {
				if (!prom_claim_page(pa)) {
					throw MsgException("cannot claim page table memory.");
				}
			}
			
			testforth();
			
			gDisplay->print("now starting client...");
			gDisplay->setAnsiColor(VCP(VC_WHITE, CONSOLE_BG));
		}
		
		void doneClient()
		{
			io_done();
			doneUI();
		}

		void startCPU(const std::shared_ptr<ClientConfig>& clientConfig)
		{
			cpu_thread_start(0, [clientConfig] {
				if (!prom_load_boot_file(clientConfig->getPromFile())) {
					ht_printf("cannot find boot file.\n");
				}
			});
		}
		
		bool isCPURunning()
		{
			return cpu_thread_is_running(0);
		}
		
		void waitForCPU()
		{
			cpu_thread_wait(0);
		}
		
	}
}

#if !defined(PEARPC_UI_PROVIDES_MAIN)
#if defined(PEARPC_UI_SDL) && defined(__APPLE__)
// On macOS SDL 1.2.15 has to run its main before handing off to SDL_main
extern "C" int SDL_main(int argc, char *argv[])
#else
int main(int argc, char *argv[])
#endif
{
	if (argc != 2) {
		usage();
	}
	setvbuf(stdout, 0, _IONBF, 0);
	
	sys_gui_init();
	
#if defined(WIN32) || defined(__WIN32__)
#else
	strncpy(gAppFilename, argv[0], sizeof gAppFilename);
#endif

	if (!initAtom()) return 3;
	if (!initData()) return 4;
	if (!initOSAPI()) return 5;
	try {
		std::shared_ptr<ClientConfig> clientConfig;
		
		try {
			clientConfig = main::loadConfig(argv[1]);
		} catch (const Exception &e) {
			String res;
			e.reason(res);
			ht_printf("%s: %y\n", argv[1], &res);
			usage();
			exit(1);
		}

		ht_printf("This program is free software; you can redistribute it and/or modify\n"
			"it under the terms of the GNU General Public License version 2 as published by\n"
			"the Free Software Foundation.\n"
			"\n"
			"This program is distributed in the hope that it will be useful,\n"
			"but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
			"MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n"
			"GNU General Public License for more details.\n"
			"\n"
			"You should have received a copy of the GNU General Public License\n"
			"along with this program; if not, write to the Free Software\n"
			"Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111 USA\n\n");

		main::initClient(clientConfig, [&clientConfig] (const char* title) {
			initUI(title, *clientConfig);
		});
		
		// begin cpu execution on a new thread
		main::startCPU(clientConfig);

		// this was your last chance to visit the config..
		clientConfig.reset();

		mainLoopUI([] () -> bool {
			return !main::isCPURunning();
		});

		// wait for cpu thread to complete
		main::waitForCPU();
		
		main::doneClient();
	} catch (const std::exception &e) {
		ht_printf("main() caught exception: %s\n", e.what());
		return 1;
	} catch (const Exception &e) {
		String res;
		e.reason(res);
		ht_printf("main() caught exception: %y\n", &res);
		return 1;
	}

	doneOSAPI();
	doneData();
	doneAtom();
	return 0;
}
#endif
