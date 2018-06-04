#include "Tile.h"
#include "Miscellaneous/strings.h" // Tile::GetFontIndex
#include "Patches/TagIDs/Main.h" // --- // Tile::GetFontIndex
#include "Services/FontShim.h" //  // Tile::GetFontIndex
#include "obse/GameAPI.h"

namespace RE {
   extern TileParseState* const g_tileParseState = (TileParseState*)0x00B3AF10;

   DEFINE_SUBROUTINE(Tile*,       GetDescendantTileByName, 0x00589B10, Tile* parent, const char* name); // searches recursively
   DEFINE_SUBROUTINE(UInt32,      GetOrCreateTempTagID,    0x0058B040, const char* traitName, SInt32 alwaysNegativeOne); // used for traits beginning with an underscore or ampersand
   DEFINE_SUBROUTINE(UInt32,      ProcessSrcAttributeVal,  0x0058B800, const char* src, char** outSrcArg); // returns trait ID; outSrcArg is set to the argument, e.g. "sibling(IT_IS_SET_TO_THIS)"
   DEFINE_SUBROUTINE(const char*, TagIDToName,             0x00589080, UInt32 tagID);
   DEFINE_SUBROUTINE(UInt32,      TagNameToID,             0x00588EF0, const char* tagName);
   DEFINE_SUBROUTINE(bool,        TileValueFormsCircularReference, 0x0058BAD0, Tile::Value*); // this also updates the values list in g_tileParseState

   Tile::TileTemplateItem* Tile::TileTemplateItem::Create(UInt32 code, float tagIdOrValue, const char* string, SInt32 result, UInt32 lineNumber) {
      auto memory = (Tile::TileTemplateItem*) FormHeap_Allocate(sizeof(Tile::TileTemplateItem));
      if (memory)
         return new (memory) Tile::TileTemplateItem(code, tagIdOrValue, string, result, lineNumber); // "placement new"
      return nullptr;
   };
   Tile::TileTemplateItem::TileTemplateItem(UInt32 code, float tagIdOrValue, const char* string, SInt32 result, UInt32 lineNumber) {
      ThisStdCall(0x00589FA0, this, code, tagIdOrValue, string, result, lineNumber);
   };
   __declspec(noinline) Tile::TileTemplateItem::~TileTemplateItem() {
      // RE::BSStringT::~BSStringT handles this:
      /*FormHeap_Free(this->string.m_data);
      this->string.m_data = nullptr;
      this->string.m_bufLen = 0;
      this->string.m_dataLen = 0;*/
   };
   bool Tile::TileTemplateItem::ExtendedStrOrNumParse() { // returns true if it's a number
      constexpr char* entities[] = {
         "&amp;",
         "&apos;",
         "&gt;",
         "&lt;",
         "&nbsp;",
         "&newline;",
         "&quot;",
      };
      constexpr char  results[]  = {
         '&',
         '\'',
         '>',
         '<',
         ' ',
         '\n',
         '"',
      };
      //
      const char* s = this->string.m_data;
      if (!s)
         return false;
      std::string copy;
      if (this->string.m_dataLen != 0xFFFF)
         copy.reserve(this->string.m_dataLen);
      //
      bool isNumber    = true;
      bool madeChanges = false;
      {
         UInt32 i = 0;
         char   c = s[i];
         if (c == '-') {
            copy += '-';
            i++;
         }
         bool foundDecimal = false;
         do {
            c = s[i];
            i++;
            //
            if (isNumber && c) { // don't trip over nulls
               if (c == '.') {
                  if (foundDecimal)
                     isNumber = false;
                  foundDecimal = true;
               } else if (c < '0' || c > '9')
                  isNumber = false;
            }
            if (c == '&') {
               UInt32 whichEntity = 0;
               bool   match  = false;
               UInt32 k;
               const char* entity;
               do {
                  entity = entities[whichEntity];
                  k = 0;
                  char   strChar;
                  do {
                     strChar = s[i + k];
                     k++;
                     //
                     if (entity[k] == '\0')
                        match = true;
                     if (entity[k] != strChar)
                        break;
                  } while (strChar);
                  if (match)
                     break;
               } while (++whichEntity < std::extent<decltype(entities)>::value);
               if (match) {
                  madeChanges = true;
                  copy += results[whichEntity];
                  i    += k - 1; // omit null-terminator
                  continue;
               }
            }
            copy += c;
         } while (c);
      }
      if (isNumber) {
         float temp = 0.0;
         sscanf(s, "%f", &temp);
         CALL_MEMBER_FN(&this->string, Replace_MinBufLen)("", 0);
         this->tagType = temp;
         this->result  = (SInt32) temp;
      } else if (madeChanges) {
         CALL_MEMBER_FN(&this->string, Replace_MinBufLen)(copy.c_str(), 0);
      }
      return isNumber;
   };

   //

   bool Tile::GetFloatValue(UInt32 traitID, float* out) {
      auto trait = CALL_MEMBER_FN(this, GetTrait)(traitID);
      if (trait && trait->bIsNum) {
         *out = trait->num;
         return true;
      }
      return false;
   };
   bool Tile::GetStringValue(UInt32 traitID, const char** out) {
      /*
      for (auto node = valueList.start; node; node = node->next) {
         auto data = node->data;
         if (data && data->id == traitID) {
            if (data->bIsNum == 0) {
               *out = data->str.m_data;
               return true;
            }
            return false;
         }
      }
      return false;
      //*/
      auto trait = CALL_MEMBER_FN(this, GetTrait)(traitID);
      if (trait && !trait->bIsNum) {
         auto str = trait->str.m_data;
         *out = str;
         if (str)
            return true;
      }
      return false;
   };
   void Tile::DeleteChildren() {
      auto children = &this->childList;
      for (auto node = children->start; node; ) {
         auto next = node->next;
         auto data = node->data;
         if (data)
            data->Dispose(true);
         //children->FreeNode(node); // DO NOT call FreeNode; the Tile destructor does that for us when the Tile removes itself from the hierarchy!
         node = next;
      }
      children->start = nullptr;
      children->end = nullptr;
      children->numItems = 0;
      CALL_MEMBER_FN(this, HandleChangeFlags)();
   };
   SInt32 Tile::GetFontIndex() {
      const char* string = CALL_MEMBER_FN(this, GetStringTraitValue)(CobbPatches::TagIDs::_traitFontPath);
      if (string && cobb::string_has_content(string)) {
         return FontShim::GetInstance().GetIndex(string);
      }
      float font = 1.0F;
      if (this->GetFloatValue(0x0FD3, &font))
         return ((SInt32) font) - 1;
      return 0;
   };
   void Tile::GetAbsoluteCoordinates(float& outX, float& outY, float& outDepth) {
      outX = CALL_MEMBER_FN(this, GetFloatTraitValue)(kTileValue_x);
      outY = CALL_MEMBER_FN(this, GetFloatTraitValue)(kTileValue_y);
      outDepth = CALL_MEMBER_FN(this, GetFloatTraitValue)(kTileValue_depth);
      auto tile = this->parent;
      if (!tile)
         return;
      do {
         if (CALL_MEMBER_FN(tile, GetFloatTraitValue)(kTileValue_locus) != 2.0F) // hm... vanilla abs-depth skips != 2, but abs-y skips == 0 ?
            continue;
         outX += CALL_MEMBER_FN(tile, GetFloatTraitValue)(kTileValue_x);
         outY += CALL_MEMBER_FN(tile, GetFloatTraitValue)(kTileValue_y);
         outDepth += CALL_MEMBER_FN(tile, GetFloatTraitValue)(kTileValue_depth);
      } while (tile = tile->parent);
   };

   void RegisterTrait(const char* trait, const SInt32 code) {
      #if OBLIVION_VERSION == OBLIVION_VERSION_1_2_416
         ThisStdCall(0x0058A0A0, nullptr, trait, code);
         _MESSAGE("Registered new trait: %08X = %s", code, trait);
      #else
         #error unsupported Oblivion version
      #endif
   }

   void Tile::Value::Expression::DebugLog(std::string* indent) const {
      std::string i = "";
      if (indent) {
         i = indent->c_str();
         i += "   ";
      }
      _MESSAGE("%s - Operator %03X with apparent value %f (%08X).", i.c_str(), this->opcode, this->operand.immediate, this->operand.ref);
      if (this->operand.immediate < 0.001 && this->operand.ref) {
         auto r = this->operand.ref;
         _MESSAGE("%s    - Apparent ref goes to a trait with ID %03X.", i.c_str(), r->id);
         auto rTile = r->owner;
         _MESSAGE("%s    - Apparent ref belongs to tile %08X.", i.c_str(), rTile);
         if ((*(UInt32*)rTile & 0xFF000000) == 0) {
            if (rTile->name.m_data) {
               _MESSAGE("%s    - Tile name is %s.", i.c_str(), rTile->name.m_data);
            }
         }
      } else if (this->refPrev) {
         _MESSAGE("%s    - Outbound reference to expression (%08X)...", i.c_str(), this->refPrev);
         this->refPrev->DebugLog(&i);
      }
      if (this->next) {
         this->next->DebugLog(indent);
      } else
         _MESSAGE("%s - End of operator-siblings.", i.c_str());
   };

   void Tile3D::SetAnimationTimePercentage(float percentage) {
      //
      // Based on the code for the enemy health bar in subroutine 0x005A8980.
      //
      NiControllerSequence* eax = this->unk44;
      if (!eax) {
         CALL_MEMBER_FN(this, UpdateAnimationState)(CALL_MEMBER_FN(this, GetStringTraitValue)(kTagID_animation));
         eax = this->unk44;
         if (!eax)
            return;
      }
      float t = eax->end;
      //
      // NOTE: Right now, we just multiply the animation end time by the percentage. 
      // Shouldn't we do ((percentage * (eax->end - eax->begin)) + eax->begin) instead? 
      // Granted, I don't know of any animations with a non-zero start time.
      //
      if (percentage <= 1.0F)
         t *= percentage;
      if (percentage < 0.0F)
         t = 0.0F;
      this->unk58 = t;
   };
}