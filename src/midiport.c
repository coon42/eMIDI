#include "midiport.h"

#include <windows.h> // TODO: move to windows HAL implementation
#include <string.h>

Error eMidi_enumOutPorts(uint32_t index, MidiPortInfo* pPortInfo) {
  int numDevs = midiOutGetNumDevs();

  if (index >= numDevs)
    return EMIDI_END_OF_PORTS;

  MIDIOUTCAPS caps;
  midiOutGetDevCaps(index, &caps, sizeof(MIDIOUTCAPS));

  pPortInfo->id = index;

#if defined(UNICODE) || defined( _UNICODE )
  memset(pPortInfo->pName, 0, sizeof(pPortInfo->pName));

  int length = WideCharToMultiByte(CP_UTF8, 0, caps.szPname, -1, NULL, 0, NULL, NULL) - 1;

  length = WideCharToMultiByte(CP_UTF8, 0, caps.szPname, (int)(wcslen(caps.szPname)),
      pPortInfo->pName, length, NULL, NULL);

#else
  strncpy(pPortInfo->pName, caps.szPname, sizeof(pPortInfo->pName));
#endif

  return EMIDI_OK;
}
