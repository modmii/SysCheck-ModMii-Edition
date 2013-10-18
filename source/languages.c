#include <stdio.h>
#include <gccore.h>
#include "sys.h"
#include "video.h"
#include "fatMounter.h"
#include "languages.h"


// 		CONF_LANG_JAPANESE
// 		CONF_LANG_ENGLISH
// 		CONF_LANG_GERMAN
// 		CONF_LANG_FRENCH
// 		CONF_LANG_SPANISH
// 		CONF_LANG_ITALIAN
// 		CONF_LANG_DUTCH
// 		CONF_LANG_SIMP_CHINESE
// 		CONF_LANG_TRAD_CHINESE
// 		CONF_LANG_KOREAN

// !!! Doens't support special chars... !!!
// !!! Don't change the order of the parameters !!!


int initLanguages(struct tm today)
{
	TXT_AppVersion = "v2.1.0 HD Edition";
	switch (CONF_GetLanguage())
	{
		case CONF_LANG_GERMAN:
			MSG_GetConsoleRegion = "Konsolen-Region wird ermittelt...";
			MSG_GetSysMenuVer = "Version des Systemsmenues wird ermittelt...";
			MSG_GetHBCVer = "Version und IOS des Homebrewkanals werden ermittelt...";
			MSG_GetRunningIOS = "Version und Revision des geladenen IOS werden ermittelt...";
			MSG_GetConsoleID = "Konsolen-ID wird ermittelt...";
			MSG_GetBoot2 = "boot2-Version wird ermittelt...";
			TXT_Region = "Region";
			TXT_Unknown = "Unbekannt";
			MSG_GetNrOfTitles = "Anzahl Titel wird ermittelt...";
			ERR_GetNrOfTitles = "Zaehlen der Titel fehlgeschlagen.";
			MSG_GetTitleList = "Liste der Titel wird generiert...";
			ERR_GetTitleList = "Die Liste konnte nicht generiert werden.";
			ERR_GetIosTMDSize = "Groesse der TMD fuer IOS%d konnte nicht ermittelt werden.";
			ERR_GetIosTMD = "Fehler beim Auslesen der TMD fuer IOS%d.";
			MSG_MountSD = "SD-Karte wird eingebunden...";
			MSG_UnmountSD = "Zugriff auf die SD-Karte wird beendet...";
			MSG_InitFAT = "Zugriff auf die SD-Karte wird initialisiert...";
			ERR_InitFAT = "Initialisieren des FAT-Systems fehlgeschlagen.";
			MSG_SortTitles = "Titel werden sortiert...";
			MSG_GetCertificates = "Zertifikate werden aus dem NAND ausgelesen...";
			ERR_GetCertificates = "Zertifikate aus dem NAND ausgelesen fehlgeschalgen.";
			TXT_SysMenu = "Systemmenue %1.1f%c (v%d)";
			TXT_SysMenu2 = "Systemmenue %s%s (v%d)";
			TXT_SysMenu3 = "Systemmenue %1.1f%c (v%d, Info: v%d %s)";
			TXT_HBF = "HomebrewFilter rev%d benutzt IOS58";
			TXT_HBC = "Homebrewkanal 1.0.%d benutzt IOS%d";
			TXT_HBC_NEW = "Homebrewkanal 1.1.0 benutzt IOS%d";
			TXT_HBC_112 = "Homebrewkanal 1.1.%d benutzt IOS%d";
			TXT_HBC_STUB = "Homebrewkanal funktionslos (Stub)";
			TXT_HBF = "Homebrew Filter rev%d benutzt IOS58";
			TXT_Hollywood = "Hollywood v0x%x";
			TXT_ConsoleID = "Konsolen-ID: %d";
			TXT_vBoot2 = "Boot2 v%u";
			TXT_NrOfTitles = "Es wurden %d Titel gefunden.";
			TXT_NrOfIOS = "Es wurden %d IOS gefunden, von denen %d funktionslos (Stub) sind.";
			TXT_AppTitle = "sysCheck %s von Double_A und R2-D2199, JoostinOnline, Nano";
			TXT_AppIOS = "...laeuft auf dem IOS%d (rev %d).";
			ERR_AllocateMemory = "Speicher fuer %d Titel konnte nicht zugewiesen werden.";
			ERR_OpenFile = "Zugriff auf die Speicherdatei des Berichts fehlgeschlagen!";
			MSG_SelectIOS = "Diese IOS werden getestet (Bitte waehlen):";
			MSG_All = "Alle";
			MSG_TestingIOS = "%s wird analysiert...";
			MSG_ReloadIOS = "Neuladen des IOS%d (rev %d)...";
			MSG_GenerateReport = "Bericht wird generiert...";
			MSG_ReportSuccess = "Erfolg! Soll der Bericht angezeigt werden?";
			MSG_ReportError = "Fehler! Soll der Bericht trotzdem angezeigt werden?";
			TXT_Stub = " Funktionslos (Stub)";
            TXT_Trucha = " Trucha Bug,";
            TXT_ES = " ES Identify,";
            TXT_Flash = " Flash Zugriff,";
            TXT_NAND = " NAND Zugriff,";
            TXT_Boot2 = " Boot2 Zugriff,";
            TXT_USB = " USB 2.0,";
			TXT_NoPatch = " Keine Patches,";
            sprintf(TXT_ReportDate, "Bericht wurde am %2.2d.%2.2d.%4.4d generiert.", today.tm_mday, today.tm_mon + 1, today.tm_year + 1900);
			BUT_HBC = "Homebrew Channel";
			BUT_Shutoff = "Ausschalten";
			BUT_SysMenu = "Systemmenue";
			BUT_Update = "Aktualisieren";
			TXT_Priiloader = "Priiloader installiert";
			TXT_PreFiix = "PreFiix installiert";
			TXT_VersionP = " ES_Version,";
			MSG_Update = "Aktualisiere SysCheck...";
			MSG_NoUpdate = "Diese Version ist bereits auf dem neuesten Stand!";
			MSG_UpdateSuccess = "Die Aktualisierung wurde erfolgreich geladen!";
			MSG_UpdateFail = "Der Aktualisierungsvorgang ist fehlgeschlagen!";
			TXT_DVD = "Laufwerksdatum: %s";
			TXT_NoDVD = "Das Laufwerksdatum konnte nicht erkannt werden";
			BUT_ConfirmUpload = "Bericht hochladen";
			TXT_Upload = "Bericht wird hochgeladen...";
			BUT_OK = "OK";
			TXT_OriginalRegion = " (urspruengliche Region: ";
			break;

		case CONF_LANG_FRENCH:
			MSG_GetConsoleRegion = "Obtention de la region de la console...";
			MSG_GetSysMenuVer = "Obtention de la version du menu systeme...";
			MSG_GetHBCVer = "Obtention de la version du Chaine Homebrew...";
			MSG_GetRunningIOS = "Obtention de la version et de la revision de l'IOS courant...";
			MSG_GetConsoleID = "Obtention de l'identifiant de la console...";
			MSG_GetBoot2 = "Obtention de la version du boot2...";
			TXT_Region = "Region";
			TXT_Unknown = "inconnue";
			MSG_GetNrOfTitles = "Obtention du nombre de titres...";
			ERR_GetNrOfTitles = "Impossible d'obtenir le nombre de titres.";
			MSG_GetTitleList = "Obtention de la liste des titres...";
			ERR_GetTitleList = "Impossible d'obtenir la liste des titres.";
			ERR_GetIosTMDSize = "Erreur lors de l'appel a ES_GetStoredTMDSize pour l'IOS%d.";
			ERR_GetIosTMD = "Erreur lors de l'appel a ES_GetStoredTMD pour l'IOS%d.";
			MSG_MountSD = "Montage de la carte SD...";
			MSG_UnmountSD = "Demontage de la carte SD...";
			MSG_InitFAT = "Initialisation du systeme de fichier FAT...";
			ERR_InitFAT = "Impossible d'initialiser le systeme de fichier FAT.";
			MSG_SortTitles = "Tri des titres...";
			MSG_GetCertificates = "Obtention des certificats de la 'NAND'...";
			ERR_GetCertificates = "Impossible d'obtenir les certificats de la 'NAND'";
			TXT_SysMenu = "Menu Systeme %1.1f%c (v%d)";
			TXT_SysMenu2 = "Menu Systeme %s%s (v%d)";
			TXT_SysMenu3 = "Menu Systeme %1.1f%c (v%d, Info: v%d %s)";
			TXT_HBC = "Chaine Homebrew 1.0.%d utilise IOS%d";
			TXT_HBC_NEW = "Chaine Homebrew 1.1.0 utilise IOS%d";
			TXT_HBC_112 = "Chaine Channel 1.1.%d utilise IOS%d";
			TXT_HBF = "Homebrew Filter rev%d utilise IOS58";
			TXT_Hollywood = "Hollywood v0x%x";
			TXT_ConsoleID = "Identifiant de la console: %d";
			TXT_vBoot2 = "Boot2 v%u";
			TXT_NrOfTitles = "%d titres trouves.";
			TXT_NrOfIOS = "%d IOS trouves sur cette console. %d sont des stubs.";
			TXT_AppTitle = "sysCheck %s par Double_A et R2-D2199, JoostinOnline, Nano";
			TXT_AppIOS = "...tourne sous l'IOS%d (rev %d).";
			ERR_AllocateMemory = "Imposible d'allouer la memoire pour %d titres.";
			ERR_OpenFile = "Impossible d'ouvrir le fichier.";
			MSG_SelectIOS = "Cet IOS sera teste (Choisissez):";
			MSG_All = "Tous";
			MSG_TestingIOS = "Teste les vulnerabilites de: %s...";
			MSG_ReloadIOS = "Recharge l'IOS%d (rev %d)...";
			MSG_GenerateReport = "Generation du rapport...";
			MSG_ReportSuccess = "Succes! Voulez vous afficher le rapport?";
			MSG_ReportError = "Erreur! Voulez vous afficher le rapport quand meme?";
            TXT_Stub = " Stub";
            TXT_Trucha = " Trucha Bug,";
            TXT_ES = " ES Identify,";
            TXT_Flash = " Acces Flash,";
            TXT_NAND = " Acces NAND,";
            TXT_Boot2 = " Acces Boot2,";
            TXT_USB = " USB 2.0,";
			TXT_NoPatch = " Pas de patches,";
			sprintf(TXT_ReportDate, "Rapport genere le %4.4d/%2.2d/%2.2d.", today.tm_year + 1900, today.tm_mon + 1, today.tm_mday);
			BUT_HBC = "Homebrew Channel";
			BUT_Shutoff = "Eteindre";
			BUT_SysMenu = "Menu systeme";
			BUT_Update = "Mise a jour";
			TXT_Priiloader = "Priiloader installe";
			TXT_PreFiix = "PreFiix installe";
			MSG_Update = "Mise a jour SysCheck...";
			MSG_NoUpdate = "Cette version est a jour!";
			MSG_UpdateSuccess = "La mise a jour a ete charge avec succes!";
			MSG_UpdateFail = "Le processus de mise a jour a echoue!";
			TXT_DVD = "Date de disque: %s";
			TXT_NoDVD = "Impossible de detecter la date d'entrainement";
			BUT_ConfirmUpload = "Charger le rapport";
			TXT_Upload = "Telechargement du rapport...";
			BUT_OK = "OK";
			TXT_OriginalRegion = " (region d'origine: ";
			break;

		case CONF_LANG_ITALIAN:
			MSG_GetConsoleRegion = "Controllo regione console...";
			MSG_GetSysMenuVer = "Controllo versione menu sistema...";
			MSG_GetHBCVer = "Controllo versione IOS e HBC...";
			MSG_GetRunningIOS = "Controllo versione IOS di appoggio...";
			MSG_GetConsoleID = "Controllo ID della console...";
			MSG_GetBoot2 = "Controllo versione Boot2...";
			TXT_Region = "Regione";
			TXT_Unknown = "sconosciuto";
			MSG_GetNrOfTitles = "Controllo numero giochi...";
			ERR_GetNrOfTitles = "Errore nel controllo numero giochi.";
			MSG_GetTitleList = "Controllo elenco giochi...";
			ERR_GetTitleList = "Errore controllo elenco giochi.";
			ERR_GetIosTMDSize = "Impossibile controllare dimensione TMD per IOS%d";
			ERR_GetIosTMD = "Impossibile verificare TMD memorizzato per IOS%d.";
			MSG_MountSD = "Controllo della memoria SD...";
			MSG_UnmountSD = "Impossibile verificare la memoria SD...";
			MSG_InitFAT = "Controllo file system FAT...";
			ERR_InitFAT = "Impossibile verificare file system FAT.";
			MSG_SortTitles = "Ordinamento giochi...";
			MSG_GetCertificates = "Controllo certificati nella NAND...";
			ERR_GetCertificates = "Errore nella verifica certificati nella NAND...";
			TXT_SysMenu = "Menu di sistema %1.1f%c (v%d)";
			TXT_SysMenu2 = "Menu di sistema %s%s (v%d)";
			TXT_SysMenu3 = "Menu di sistema %1.1f%c (v%d, Info: v%d %s)";
			TXT_HBC = "Canale Homebrew 1.0.%d appoggiato all'IOS%d";
			TXT_HBC_NEW = "Canale Homebrew 1.1.0 appoggiato all'IOS%d";
			TXT_HBC_112 = "Canale Homebrew 1.1.%d appoggiato all'IOS%d";
			TXT_HBF = "Homebrew Filter rev%d appoggiato all'IOS58";
			TXT_Hollywood = "Hollywood v0x%x";
			TXT_ConsoleID = "ID console: %d";
			TXT_vBoot2 = "Boot2 v%u";
			TXT_NrOfTitles = "Trovati %d giochi.";
			TXT_NrOfIOS = "Trovati %d IOS in questa console. %d di questi sono stub.";
			TXT_AppTitle = "sysCheck %s by Double_A and R2-D2199, JoostinOnline, Nano";
			TXT_AppIOS = "...appoggiato all'IOS%d (v. %d).";
			ERR_AllocateMemory = "Impossibile allocare la memoria per %d giochi.";
			ERR_OpenFile = "Impossibile aprire il file!";
			MSG_SelectIOS = "Verranno testati questi IOS:";
			MSG_All = "Tutti";
			MSG_TestingIOS = "Verifica vulnerabilita' %s...";
			MSG_ReloadIOS = "Ricaricamento IOS%d (v. %d)...";
			MSG_GenerateReport = "Creazione del rapporto...";
			MSG_ReportSuccess = "Fatto! Vuoi vedere il rapporto?";
			MSG_ReportError = "Errore! Vuoi vedere ugualmente il rapporto?";
            TXT_Stub = " Stub";
            TXT_Trucha = " Trucha Bug,";
            TXT_ES = " Identificazione ES,";
            TXT_Flash = " Accesso flash,";
            TXT_NAND = " Accesso NAND,";
            TXT_Boot2 = " Accesso Boot2,";
            TXT_USB = " USB 2.0,";
			TXT_NoPatch = " Non patchato,";
			sprintf(TXT_ReportDate, "Rapporto generato il %2.2d.%2.2d.%4.4d.", today.tm_mday, today.tm_mon + 1, today.tm_year + 1900);
			BUT_HBC = "Canale Homebrew";
			BUT_Shutoff = "Spegni";
			BUT_SysMenu = "Menu di sistema";
			BUT_Update = "Aggiorna";
			TXT_Priiloader = "Priiloader installato";
			TXT_PreFiix = "PreFiix installato";
			MSG_Update = "Aggiornamento SysCheck...";
			MSG_NoUpdate = "Questa versione e' aggiornata!";
			MSG_UpdateSuccess = "L'aggiornamento e' stato caricato correttamente!";
			MSG_UpdateFail = "Il processo di aggiornamento non e' riuscito!";
			TXT_DVD = "Data unita': %s";
			TXT_NoDVD = "Impossibile rilevare la data dell'unita'";
			BUT_ConfirmUpload = "Invia rapporto";
			TXT_Upload = "Invio rapporto...";
			BUT_OK = "OK";
			TXT_OriginalRegion = " (regione originale: ";
		    break;

		case CONF_LANG_SPANISH:
			MSG_GetConsoleRegion = "Obteniendo la region de la consola...";
			MSG_GetSysMenuVer = "Obteniendo la version del Menu de Sistema...";
			MSG_GetHBCVer = "Obteniendo version de Canal Homebrew y IOS que usa...";
			MSG_GetRunningIOS = "Obteniendo version y revision de IOS usado...";
			MSG_GetConsoleID = "Obteniendo ID de la consola...";
			MSG_GetBoot2 = "Obteniendo la version de boot2...";
			TXT_Region = "Region";
			TXT_Unknown = "desconocido";
			MSG_GetNrOfTitles = "Obteniendo numero de titulos...";
			ERR_GetNrOfTitles = "No se pudo obtener numero de titulos.";
			MSG_GetTitleList = "Obteniendo lista de titulos...";
			ERR_GetTitleList = "No se pudo obtener lista de titulos.";
			ERR_GetIosTMDSize = "Error al llamar ES_GetStoredTMDSize de IOS%d.";
			ERR_GetIosTMD = "Error al llamar ES_GetStoredTMD de IOS%d.";
			MSG_MountSD = "Montando Tarjeta SD...";
			MSG_UnmountSD = "Desmontando Tarjeta SD...";
			MSG_InitFAT = "Inicializando sistema de archivos FAT...";
			ERR_InitFAT = "Imposible inicializar sistema de archivos FAT.";
			MSG_SortTitles = "Ordenando titulos...";
			MSG_GetCertificates = "Obteniendo certificados de la NAND...";
			ERR_GetCertificates = "No se pudieron obtener certificados de la NAND...";
			TXT_SysMenu = "Menu de Sistema %1.1f%c (v%d)";
			TXT_SysMenu2 = "Menu de Sistema %s%s (v%d)";
			TXT_SysMenu3 = "Menu de Sistema %1.1f%c (v%d, Info: v%d %s)";
			TXT_HBC = "Canal Homebrew 1.0.%d corriendo en IOS%d";
			TXT_HBC_NEW = "Canal Homebrew 1.1.0 corriendo en IOS%d";
			TXT_HBC_112 = "Canal Homebrew 1.1.%d corriendo en %d";
			TXT_HBF = "Homebrew Filter rev%d corriendo en IOS58";
			TXT_Hollywood = "Hollywood v0x%x";
			TXT_ConsoleID = "ID de consola: %d";
			TXT_vBoot2 = "Boot2 v%u";
			TXT_NrOfTitles = "Encontrados %d titulos.";
			TXT_NrOfIOS = "Se encontraron %d IOS en esta consola. %d de ellos son stubs.";
			TXT_AppTitle = "sysCheck %s por Double_A y R2-D2199, JoostinOnline, Nano";
			TXT_AppIOS = "...corre en IOS%d (rev %d).";
			ERR_AllocateMemory = "Imposible asignar memoria para %d titulos.";
			ERR_OpenFile = "Imposible abrir el archivo!";
			MSG_SelectIOS = "Este IOS sera analizado (Selecciona):";
			MSG_All = "Todos";
			MSG_TestingIOS = "Analizando %s...";
			MSG_ReloadIOS = "Recargando IOS%d (rev %d)...";
			MSG_GenerateReport = "Generando reporte...";
			MSG_ReportSuccess = "Exito! Quieres ver el reporte?";
			MSG_ReportError = "Error! Quieres mostrar el reporte igualmente?";
			TXT_Stub = " Stub";
			TXT_Trucha = " Trucha Bug,";
			TXT_ES = " ES Identify,";
			TXT_Flash = " Acceso Flash,";
			TXT_NAND = " Acceso NAND,";
			TXT_Boot2 = " Acceso Boot2,";
			TXT_USB = " USB 2.0,";
			TXT_NoPatch = " Sin Parches,";
			sprintf(TXT_ReportDate, "Reporte generado el %2.2d/%2.2d/%4.4d.", today.tm_mday, today.tm_mon + 1, today.tm_year + 1900);
			BUT_HBC = "Canal Homebrew";
			BUT_Shutoff = "Apagar";
			BUT_SysMenu = "Menu de Sistema";
			BUT_Update = "Actualizacion";
			TXT_Priiloader = "Priiloader instalado";
			TXT_PreFiix = "PreFiix instalado";
			MSG_Update = "Actualizacion SysCheck...";
			MSG_NoUpdate = "Esta version esta actualizada!";
			MSG_UpdateSuccess = "La actualizacion se ha cargado con exito!";
			MSG_UpdateFail = "El proceso de actualizacion ha fallado!";
			TXT_DVD = "Unidad de la fecha: %s";
			TXT_NoDVD = "No se ha detectado la fecha de la unidad";
			BUT_ConfirmUpload = "Subir informe";
			TXT_Upload = "Cargar el informe...";
			BUT_OK = "OK";
			TXT_OriginalRegion = " (region de origen: ";
			break;

		default:
			MSG_GetConsoleRegion = "Getting the console region...";
			MSG_GetSysMenuVer = "Getting the system menu version...";
			MSG_GetHBCVer = "Getting the homebrew channel version and IOS...";
			MSG_GetRunningIOS = "Getting the running IOS version and revision...";
			MSG_GetConsoleID = "Getting the console ID...";
			MSG_GetBoot2 = "Getting the boot2 version...";
			TXT_Region = "Region";
			TXT_Unknown = "unknown";
			MSG_GetNrOfTitles = "Getting number of titles...";
			ERR_GetNrOfTitles = "Failed getting number of titles.";
			MSG_GetTitleList = "Getting list of titles...";
			ERR_GetTitleList = "Failed getting list of titles.";
			ERR_GetIosTMDSize = "Failed to get the stored TMD size for IOS%d";
			ERR_GetIosTMD = "Failed to get the stored TMD for IOS%d.";
			MSG_MountSD = "Mounting the SD Card...";
			MSG_UnmountSD = "Unmounting the SD Card...";
			MSG_InitFAT = "Initialising the FAT file system...";
			ERR_InitFAT = "Unable to initialise the FAT file system.";
			MSG_SortTitles = "Sorting titles...";
			MSG_GetCertificates = "Getting the certificates from the NAND...";
			ERR_GetCertificates = "Failed getting the certificates from the NAND...";
			TXT_SysMenu = "System Menu %1.1f%c (v%d)";
			TXT_SysMenu2 = "System Menu %s%s (v%d)";
			TXT_SysMenu3 = "System Menu %1.1f%c (v%d, Info: v%d %s)";
			TXT_HBC = "Homebrew Channel 1.0.%d running on IOS%d";
			TXT_HBC_NEW = "Homebrew Channel 1.1.0 running on IOS%d";
			TXT_HBC_112 = "Homebrew Channel 1.1.%d running on IOS%d";
			TXT_HBF = "Homebrew Filter rev%d running on IOS58";
			TXT_Hollywood = "Hollywood v0x%x";
			TXT_ConsoleID = "Console ID: %d";
			TXT_vBoot2 = "Boot2 v%u";
			TXT_NrOfTitles = "Found %d titles.";
			TXT_NrOfIOS = "Found %d IOS on this console. %d of them are stub.";
			TXT_AppTitle = "sysCheck %s by Double_A and R2-D2199, JoostinOnline, Nano";
			TXT_AppIOS = "...runs on IOS%d (rev %d).";
			ERR_AllocateMemory = "Unable to allocate the memory for %d titles.";
			ERR_OpenFile = "Unable to open the file!";
			MSG_SelectIOS = "This IOS will be tested (Please select):";
			MSG_All = "All";
			MSG_TestingIOS = "Testing %s vulnerabilities...";
			MSG_ReloadIOS = "Reloading IOS%d (rev %d)...";
			MSG_GenerateReport = "Generating the report...";
			MSG_ReportSuccess = "Success! Do you want to display the report?";
			MSG_ReportError = "Error! Do you want to display the report anyway?";
            TXT_Stub = " Stub";
            TXT_Trucha = " Trucha Bug,";
            TXT_ES = " ES Identify,";
            TXT_Flash = " Flash Access,";
            TXT_NAND = " NAND Access,";
            TXT_Boot2 = " Boot2 Access,";
            TXT_USB = " USB 2.0,";
			TXT_NoPatch = " No Patches,";
			sprintf(TXT_ReportDate, "Report generated on %2.2d/%2.2d/%4.4d.", today.tm_mon + 1, today.tm_mday, today.tm_year + 1900);
			BUT_HBC = "Homebrew Channel";
			BUT_Shutoff = "Shut down";
			BUT_SysMenu = "System Menu";
			BUT_Update = "Update";
			TXT_Priiloader = "Priiloader installed";
			TXT_PreFiix = "PreFiix installed";
			MSG_Update = "Updating SysCheck...";
			MSG_NoUpdate = "This version is up to date!";
			MSG_UpdateSuccess = "The update was successfully loaded!";
			MSG_UpdateFail = "The update process has failed!";
			TXT_DVD = "Drive date: %s";
			TXT_NoDVD = "Could not detect the drive date!";
			BUT_ConfirmUpload = "Upload report";
			TXT_Upload = "Uploading report...";
			BUT_OK = "OK";
			TXT_OriginalRegion = " (original region: ";
		}

	return 0;
}
