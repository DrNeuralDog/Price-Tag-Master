[2025-08-08 00:00:00] User requested structured PRD rewrite (Qt 6, Excel API) - PRD updated in docs/PRD.md
[2025-08-08 00:10:00] User requested structured CR creation (Qt C++ project) - CR created in docs/CR.md
[2025-08-08 00:20:00] User requested base project documentation - Implementation, structure, and UI/UX docs created; task checklist updated

[2025-08-08 00:30:00] User requested Light/Dark UI design deliverables for Figma - Created tokens and spec files (docs/Figma_Tokens_LightDark.json, docs/Figma_Spec_LightDark.md)

[2025-08-08 00:40:00] User asked for Figma import instructions - Step-by-step guide provided

[2025-08-08 00:45:00] User requested alternative (non-Figma) design preview - Added offline HTML preview at docs/DesignPreview/index.html
[2025-08-30 00:00:00] User asked to auto-apply new app icon `@PriceTagMangerIcon.jpg` - Implemented: resource added to `resources/resources.qrc`, app/window icon set in `src/main.cpp`; build skipped by user
[2025-08-30 00:15:00] User provided `.ico`; requested proper Windows EXE icon - Implemented .rc (`resources/windows/app_icon.rc`), added to CMake; compile step skipped by user

[2025-08-08 12:00:00] User requested two UI themes implementation (Light/Dark) - Implemented ThemeManager, toolbar toggle, persisted setting
[2025-08-09 10:00:00] User reported theme/UX mismatch with Figma spec - Applied token-based QPalette+QSS, updated toolbar/dropzone/button styles; theme toggle persists - Fixed
[2025-08-12 00:00:00] User requested Stage 2 — correct XLSX export - Implemented mm-based A4 grid, correct column/row sizing, and print area in Excel export - Success
[2025-08-12 13:30:00] User requested Stage 3 — correct DOCX export (match PriceTagExample.png) - Implemented OpenXML DOCX writer with A4 pages and grid; awaiting user build verification
[2025-08-14 00:00:00] User asked to apply single outer border for whole XLSX price tag and remove duplicate edge formatting from inner cells - Implemented in src/excelgenerator.cpp
[2025-08-14 14:00:00] User requested precise XLSX cell dimensions (cols: 7, 3.57, 3.57, 3.43; rows: 16.5, 16.5, 16.5, 12.75, 12.75, 12.75, 15.75, 16.5, 10.5, 13.5, 9.75, 9.75) - Updated implementation in src/excelgenerator.cpp - Done
[2025-08-14 14:20:00] User requested fine-tune: col1 ≈ 7.71 cm, col4 ≈ 2.71 cm; split Supplier row into two cells; for two prices remove label and strike old price with diagonal slash - Implemented in src/excelgenerator.cpp - Done
[2025-08-14 14:40:00] User requested same behavior for DOCX: mirror Excel layout and two-price formatting - Implemented in src/wordgenerator.cpp - Done
[2025-08-26 00:00:00] User requested DOCX updates: keep whole price tag on one page; make outer borders medium - Implemented non-splitting rows and medium borders in src/wordgenerator.cpp; adjusted tag size in include/wordgenerator.h - Success
[2025-08-26 00:35:00] User requested rollback of scaling and only move last non-fitting row to next page - Reverted sizes to 175.6×57.6 mm, removed scaling, kept single outer table with w:cantSplit rows - Applied
[2025-08-28 00:00:00] User requested: finish Template Editor to control per-field fonts/sizes and apply to DOCX/XLSX - Work started, template model and editor UI extended; wiring to generators implemented
[2025-09-01 00:00:00] User requested: move Size to its own column but render it in Category row; center first line by default; fix address two-line wrapping to avoid word loss; remove signature text - Implemented in src/excelgenerator.cpp, src/wordgenerator.cpp, include/tagtemplate.h, src/templateeditor.cpp - Pending user build verification
[2025-09-09 00:00:00] User requested: read Size from column "Размер" and append after Gender in the Category row (same output cell) - Implemented in src/excelgenerator.cpp and src/wordgenerator.cpp; parsing already supports "Размер"