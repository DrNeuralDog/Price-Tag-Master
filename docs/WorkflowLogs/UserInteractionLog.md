[2025-08-08 00:00:00] User requested structured PRD rewrite (Qt 6, Excel API) - PRD updated in docs/PRD.md
[2025-08-08 00:10:00] User requested structured CR creation (Qt C++ project) - CR created in docs/CR.md
[2025-08-08 00:20:00] User requested base project documentation - Implementation, structure, and UI/UX docs created; task checklist updated

[2025-09-20 15:05:00] User asked to revert tab font weight in Light theme; keep only color tweaks - Applied: removed bold from QTabBar::tab (light), retained updated color tokens
[2025-09-20 15:12:00] User requested stronger visual emphasis in Light theme - Applied: added input focus/hover states, stronger selected/hover tabs, GroupBox title accent, scrollbar hover, button focus border
[2025-09-20 15:18:00] User asked to emphasize drag-and-drop area - Applied: thicker dashed border (3px), larger radius (14px), light gradient background; hover/success preserved
[2025-09-20 15:25:00] User requested rollback of drop area visuals with darker gray border and brighter gray fill - Applied: reverted size/radius; border to #CBD5E1, fill to #F1F5F9; kept hover/success colors
[2025-09-20 14:51:00] User reported build failure (QtCharts types + drag/drop API). Applied namespace macro and Qt5/Qt6 DnD fix; pending rebuild - Changes applied
[2025-08-08 00:30:00] User requested Light/Dark UI design deliverables for Figma - Created tokens and spec files (docs/Figma_Tokens_LightDark.json, docs/Figma_Spec_LightDark.md)

[2025-08-08 00:40:00] User asked for Figma import instructions - Step-by-step guide provided

[2025-08-08 00:45:00] User requested alternative (non-Figma) design preview - Added offline HTML preview at docs/DesignPreview/index.html

[2025-09-19 00:00:00] User reported QtCharts compile errors (QtCharts namespace/types) in `mainwindow` - Revised fix: removed `QT_CHARTS_USE_NAMESPACE` (Qt6), added `<QtCharts/QChart>`, kept unqualified chart types, removed `using namespace QtCharts` - Success
[2025-09-22 13:05:00] User reported 200+ build errors after project move - Applied fix: removed `QT_CHARTS_USE_NAMESPACE` usage in `include/mainwindow.h`, added `QtCharts::` qualifiers for member types, limited `using namespace QtCharts` to `updateCharts()`; awaiting rebuild - Pending

[2025-09-19 00:02:00] Requested permission to commit QtCharts fix to git - Pending
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
[2025-09-18 00:00:00] User requested: limit drag-and-drop to Main tab, add charts in Statistics - Limited DnD to Main; prepared charts scaffolding behind USE_QT_CHARTS flag - In progress
[2025-09-18 00:05:00] Requested user confirmation to modify CMake (enable Qt Charts module, add USE_QT_CHARTS define) - Pending user confirmation
[2025-09-18 16:10:00] User reported old icon persists after rebuild - Investigating (likely Windows icon cache/exe name/version caching); provided remediation steps
[2025-09-18 16:20:00] User approved enabling Qt Charts; CMake updated to link Charts and define USE_QT_CHARTS - Success
[2025-09-18 16:35:00] User requested modern rounded UI and dark gradient primary; applied global rounding, dark gradient for primary, silver drop area in light; replaced Template Editor text with gear icon - Success
[
2025-09-16 00:00:00] User requested template config persistence (JSON/YAML) - Implemented JSON auto load/save (ConfigManager); initial path doc/AppData fallback - Success
[
2025-09-16 00:10:00] User requested config file to always live beside the EXE - Updated ConfigManager to always use <exe_dir>\template.json - Success
[
2025-09-16 00:20:00] User requested full template serialization and rename - Emit all geometry + per-field styles (font, size, bold, italic, strike, align) and rename file to <exe_dir>\TagTemplate.json - Success
[
2025-09-16 00:30:00] User requested explicit Save button in Template Editor - Added Save button (disabled until changes), persists to <exe_dir>\TagTemplate.json on click - Success
[
2025-09-16 00:40:00] User requested drop area color states (hover blue, success green, error reset) - Implemented hover/success/default styles and dragLeave handling in MainWindow - Success
[
2025-09-18 17:20:00] User requested interactive field highlight and in-canvas resizing in Template Editor - Implemented hover/selection, click-to-select syncing with style panel, and tag resize handle; per-field resizing proposed for next step - Success
[
2025-09-18 17:28:00] User requested Ctrl+mouse wheel zoom and proper cursors (arrow over fields, resize on edges) - Implemented Ctrl+wheel zoom, arrow cursor, edge/BR resize cursors; fixed segfault on field click - Success
[2025-09-19 12:00:00] User requested language switching mechanism, square rounded lang button with spacing, and default export to Excel - Implemented via code edits; language dynamic switch suggests restart - Success
[2025-09-19 13:00:00] User reported compile error: No member 'setUiLanguage' in MainWindow - Added declaration in header and implementation in cpp - Fixed
[2025-09-19 14:00:00] User requested full Russian translations for all UI texts - Generated .ts/.qm, added to resources, wrapped remaining strings in tr() - Success
[2025-09-20 16:05:00] User requested: unify tab sizes (Main/Statistics), align Generate button height to Open, restrict editor resize to top-left tag only, and darken light-theme drop zone - Applied edits in ThemeManager/MainWindow/TemplateEditor - Success