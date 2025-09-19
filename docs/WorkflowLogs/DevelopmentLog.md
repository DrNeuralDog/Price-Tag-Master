[2025-09-09 00:00:00] Adjusted output: Size is appended only after Gender in Category line; ensured reading from column "Размер" remains active - Success
[2025-08-08 00:00:00] Created AI-friendly PRD in `docs/PRD.md`; added logs; prepared for git commit - Success
[2025-08-08 00:10:00] Authored structured CR aligned with PRD; saved to `docs/CR.md`; updated interaction logs - Success
[2025-08-08 00:20:00] Created base docs: `docs/Implementation.md`, `docs/project_structure.md`, `docs/UI_UX_doc.md`; updated task checklist - Success

[2025-08-08 00:30:00] Authored Figma-ready theme tokens and UI spec for Light/Dark; saved `docs/Figma_Tokens_LightDark.json` and `docs/Figma_Spec_LightDark.md` - Success

[2025-08-08 00:40:00] Documented Figma import workflow for Light/Dark tokens and specs - Success

[2025-08-08 00:45:00] Implemented offline Light/Dark design preview HTML at `docs/DesignPreview/index.html` - Success

[2025-08-08 12:00:00] Implemented application Light/Dark themes with toolbar toggle, persistence via QSettings; added `ThemeManager` - Success
[2025-08-09 10:00:00] Refined theming per Figma tokens: added global QSS mapping (buttons, inputs, tabs, toolbar), improved dropzone/primary button styles; updated `ThemeManager` and `MainWindow` - Success
[2025-08-09 10:20:00] Added `TemplateEditorWidget` (A4 mm-based grid) and `TemplateEditorDialog`; replaced Preview tab with Template Editor launcher - Success
[2025-08-09 10:35:00] Fixed undefined symbol after Preview removal (`refreshPreviewButton`) in `src/mainwindow.cpp` - Success
[2025-08-12 00:00:00] Implemented A4-aware XLSX export layout in `ExcelGenerator`: mm-based geometry, grid fit (N_cols/N_rows), calibrated column/row sizing, pagination, and print area definition - Success
[2025-08-12 00:20:00] Adjusted Excel export per exact client spec: fixed column widths (7, 3.57, 3.57, 3.43), row heights (15; 15.75; 15.75; 4×12.75; 15.75; 10.5; 4×13.5), restored "Артикул:" label, and expanded address to 3 lines - Success
[2025-08-12 00:40:00] Removed gaps between tags (tight grid), replaced all dashed borders with solid; applied thin/medium/thick border weights per row type; set bold+italic for Страна/Место/Матер-л/Артикул headers and last 3 lines; ensured bold for article value and price cells - Success
[2025-08-12 13:30:00] Implemented DOCX export via OpenXML in `WordGenerator`: A4 page, mm→twips mapping, nCols/nRows grid, 13-row inner table per tag mirroring XLSX; updated UI to save .docx - Pending validation
[2025-08-14 00:00:00] Adjusted XLSX tag borders: removed thick borders from inner cell formats; added helper to draw thick outer border only for the whole tag; applied formats to all merged rows - Success
[2025-08-14 00:05:00] Shortened address block in XLSX tag from 3 lines to 2 lines (concatenate tail into second line); preserved bottom border row for outer frame - Success
[2025-08-14 00:15:00] Improved address layout: word-based fitting into two lines with truncation; removed formatting from the last (border-only) row to avoid fill - Success
[2025-08-14 14:00:00] Updated XLSX tag cell dimensions in `src/excelgenerator.cpp`: column widths set to [7, 3.57, 3.57, 3.43]; row heights set to [16.5, 16.5, 16.5, 12.75, 12.75, 12.75, 15.75, 16.5, 10.5, 13.5, 9.75, 9.75] - Success
[2025-08-14 14:20:00] Tweaked widths calibration to reach ~7.71 cm for col1 and ~2.71 cm for col4; split Supplier row into label+value cells; implemented two-price logic: left cell shows only old price number with strikethrough and diagonal slash, right cell shows new price - Success
[2025-08-14 14:40:00] Mirrored Excel changes in DOCX export (`src/wordgenerator.cpp`): updated row heights; introduced 4-column inner grid with widths [77.1, 35.7, 35.7, 27.1] mm; split Supplier row; two-price logic with strike and diagonal TL→BR on old price; kept single-cell rows merged otherwise - Success
[2025-08-26 00:00:00] DOCX export polish: prevented row splitting across pages (whole tag row moves), set outer borders to medium (w:sz=8), and aligned tag dimensions to 175.6×57.6 mm; updated `src/wordgenerator.cpp` and `include/wordgenerator.h` - Success
[2025-08-28 00:10:00] Added TagTemplate model (geometry + per-field text styles) in `include/tagtemplate.h`; extended TemplateEditor with style panel and `templateChanged` signal - Success
[2025-08-28 00:20:00] Wired Template Editor to MainWindow; propagated template to Word/Excel generators; mapped geometry to layout configs - Success
[2025-08-28 00:30:00] Applied per-field fonts/sizes/alignment to DOCX export (paragraphWithStyle, inner table rendering) - Success
[2025-08-28 00:40:00] Applied per-field fonts/sizes/alignment to XLSX export formats; strikeout and diagonal slash for old price retained - Success
[2025-08-28 01:00:00] Enhanced Template Editor preview: WYSIWYG 4×12 grid with inner lines, all fields drawn per TagTemplate; added zoom controls (±, slider, Fit Page) - Success
[2025-08-30 00:00:00] Integrated application icon: added `resources/icons/PriceTagMangerIcon.jpg` to `resources/resources.qrc` and set app/main window icon via QIcon in `src/main.cpp` - Success
[2025-08-30 00:15:00] Switched app/window icons to .ico; added Windows resource `resources/windows/app_icon.rc`; wired into `CMakeLists.txt` to embed icon in EXE - Success
[2025-09-01 00:00:00] Category+Gender+Size row: appended Size from new column; default centered CompanyHeader; improved address wrapping to 2 lines without word loss; removed signature text in Excel/DOCX/Preview - Success
[2025-09-18 16:45:00] Light theme: added blue vertical gradient for primary button; Dark theme: violet-pink diagonal gradient; Fixed window icon by composing multi-size QIcon - Success
[2025-09-18 17:00:00] Updated light theme primary gradient to diagonal with lower contrast; added toolbar gear icon (SettingsGear.png) and tag icon on Generate; preferred PNG app icon for titlebar - Success
[2025-09-18 00:00:00] Limited drag-and-drop to Main tab only; added Statistics charts scaffolding (Qt Charts behind USE_QT_CHARTS) - Success (charts pending CMake enable)
[
2025-09-18 17:20:00] Template Editor interactivity: added field overlays with hover/selection highlight, click-to-select sync with style panel, and bottom-right resize handle for tag; updating spin boxes triggers scene rebuild - Success
[
2025-09-18 17:28:00] Template Editor UX: Ctrl+mouse wheel zoom; arrow cursor over fields; resize cursors on tag edges/BR corner; safe overlay cleanup preventing segfault - Success
[2025-09-18 17:45:00] Template Editor: Added per-field text editing via double click and a new Text input in Typography; wired to TagTemplate texts and live preview - Success

[2025-09-19 00:00:00] Fixed QtCharts compile errors: added QT_CHARTS_USE_NAMESPACE in `include/mainwindow.h`, switched member types to `QChartView*`, removed `using namespace QtCharts;` from `src/mainwindow.cpp` - Success
[2025-09-19 12:00:00] Implemented UI language toggle (EN/RU) with QTranslator and settings persistence; made langButton square (28x28) with 14px spacer to themeButton; changed default export to XLSX by swapping combo items - Success