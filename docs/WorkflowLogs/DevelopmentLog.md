[2025-09-25 01:05] Excel: внедрён RichString с невидимыми '*' (белый цвет) для имитации ведущих пробелов - Успех
[2025-09-25 01:09] Excel: эксперимент — замена ведущих пробелов на буквы 'П' для диагностики - Успех
[2025-09-25 00:58] Excel: дополнительный обход — ведущие пробелы заменяются на hair space (U+200A) после учёта indent - Успех
[2025-09-25 00:52] Excel: ведущие пробелы сохраняются через indent по их числу; текст пишется без лидирующих пробелов - Успех
[2025-09-25 00:41] Сохранение ведущих пробелов: Word — через xml:space="preserve" (уже было), Excel — замена ведущих пробелов на NBSP - Успех
[2025-09-25 00:28] Excel: метки берутся из шаблона; добавлен отступ для левых абзацев; внутренние границы тонкие, внешние усиливаются выборочно - Успех
[2025-09-25 00:24] Генератор DOCX теперь использует тексты из шаблона для меток (Компания, Страна, Место, Матер-л, Артикул, Цена, Поставщик) - Успех
[2025-09-25 00:18] Добавлен дефолтный отступ 1 мм для всех левовыравненных абзацев в DOCX - Успех
[2025-09-25 00:15] Уменьшена толщина правой внешней границы таблицы на 25% (16→12) - Успех
[2025-09-25 00:12] Откат центрирования/усадки; утолщена только правая внешняя граница таблицы; поля страницы возвращены - Успех
[2025-09-25 00:05] Выровнены правые/нижние границы: уменьшена ширина внутренних таблиц и добавлена центровка; увеличены правый/нижний поля секции на ~1pt - Успех
[2025-09-25 00:00] Исправлен генератор DOCX: фиксированная раскладка таблиц, масштаб колонок по ширине ячейки тега - Успех
[2025-09-20 15:05:10] Light theme polish: boosted primary colors, refined dropArea states; reverted tab font weight to previous style - Success
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
[2025-09-19 13:00:00] Fixed missing setUiLanguage method: added to MainWindow header and cpp with updateLanguageTexts call - Success
[2025-09-19 14:00:00] Added Russian translations: ran lupdate/lrelease, edited .ts with translations, embedded .qm in qrc, ensured all UI strings use tr() - Success
[2025-09-20 15:06:00] Investigated DOCX open issue; added relationships to docProps and inserted <w:p/> before <w:sectPr>; ready to rebuild and retest - Changes applied
[2025-09-20 16:05:00] Tabs unified to dark-size metrics; primary button height aligned across themes; light theme drop area background slightly darker; editor resize limited strictly to top-left tag - Success
[2025-09-20 16:12:00] Enforced equal heights for Open/Generate/Refresh buttons per theme (32/34px) in updateButtonsPrimaryStyles - Success
[2025-09-22 13:05:30] Fixed massive build break on new machine: removed `QT_CHARTS_USE_NAMESPACE` from header, scoped QtCharts to function, and qualified chart view members with `QtCharts::QChartView*`; advise rebuild - Applied
[2025-09-20 16:18:00] Restored continuous resize for top-left tag (right/bottom/BR), kept non-resizable for others - Success
[2025-09-22 00:00:00] Adjusted toolbar spacing to exact 2 mm using QToolBar padding (top/bottom) set via DPI-based pixels; buttons and bottom border aligned as requested - Success
[2025-09-22 00:10:00] Ensured 2mm drop for toolbar items: wrapped theme/lang buttons in 2mm-margin containers; added 2mm top/bottom margin to QToolBar QToolButton (gear) - Success
[2025-09-22 00:20:00] Fixed disappearing toolbar bottom line on theme toggle by removing toolbar padding and setting only border-bottom + toolbutton margins in updateThemeStyles - Success
[2025-09-22 00:25:00] Restored template editor gear button size: set QToolBar QToolButton min size to 28×28 so it matches lang button; margins kept at 2mm - Success
[2025-09-22 00:35:00] Precisely matched gear button to lang button: set QToolButton icon and fixed size to 28×28 via widgetForAction; constrained QToolBar QToolButton padding/border to avoid oversized click area - Success
[2025-09-22 00:50:00] Increased gear icon to previous visual size: cropped transparent margins and set toolbutton fixed/icon size to 32×32, updated icon on theme switch accordingly - Success
[2025-09-22 01:05:00] Enlarged gear button/icon to 52×52 (1.25× from 42) and set transparent background to match darker UI shade; vertical toolbar margins unchanged - Success
[2025-09-22 01:12:00] Unified toolbar background with window color and made toolbuttons fully transparent to eliminate any background tint mismatch; kept 2mm vertical margins - Success
[2025-09-22 01:25:00] Reduced excessive vertical hit area for gear button by wrapping 52×52 toolbutton in 1mm top/bottom margins; added 2mm toolbar right margin to avoid edge sticking - Success
[2025-09-22 02:00:00] Constrained gear button hit area strictly to icon using alpha mask in MainWindow; mask reapplied on theme change - Success
[2025-09-22 02:08:00] Reverted gear button to rectangular hit area to avoid invisibility on toolbar; retained 52×52 sizing and cropped icon - Success
[2025-09-22 02:15:00] Limited gear button clickable area horizontally by 2mm on both sides via eventFilter; avoided masks for stable visuals - Success
[2025-09-22 02:25:00] Replaced eventFilter approach with custom toolbutton (override hitButton) trimming 4mm on each side; added as toolbar widget with defaultAction - Success
[2025-09-25 00:35] Excel: ширина ценника теперь масштабируется от шаблона; уменьшен левый отступ (~0.5мм) - Успех
[2025-09-25 00:45] Excel: сохранение ведущих пробелов усилено для всех полей (метки и значения) через NBSP - Успех
[2025-09-25 01:20] Excel: в шаблонных текстах ведущие пробелы заменяются на 'П' при записи - Успех
[2025-09-25 01:28] Excel: заменил 'П' на невидимые '*' через RichString (белый цвет) - Успех
[2025-09-25 01:50] Template Editor: добавлена кнопка Reset (снизу слева). По нажатию шаблон сбрасывается к значениям по умолчанию, превью обновляется, конфиг TagTemplate.json сохраняется; локализация RU/EN поддержана - Успех
[2025-09-25 02:05] DnD: добавлен обработчик dragMoveEvent; при первом переносе Excel-файла над зоной сброса событие принимается сразу, hover-стиль включается без «разблокировки» - Успех
[2025-09-26 13:52] Сборка Qt5 MinGW падала: 'QChartView does not name a type' - Исправил: в `include/mainwindow.h` заменил поля на `QtCharts::QChartView*`; в `src/mainwindow.cpp` ограничил `using namespace QtCharts` внутри `updateCharts()`; готово к пересборке
[2025-09-29 17:12:30] Qt5/Qt6 Charts совместимость: в `include/mainwindow.h` поля `QChartView*` заменены на `QWidget*` для исключения зависимостей в заголовке; в `src/mainwindow.cpp` создаю локальные `QtCharts::QChartView`/`QChart`/`QBarSeries` и присваиваю в поля; `using namespace QtCharts` применяется только под Qt5 через `#if QT_VERSION < 6`. Готово к пересборке
[2025-09-26 14:05] DnD UX: первый перенос показывал красный запрет и принимался только после нескольких попыток - Исправлено: в dragEnter сразу принимаем valid .xlsx, hover включаем только над `dropArea`; в dragMove при выходе за пределы зоны выставляем IgnoreAction и возвращаем дефолтный стиль
[2025-09-26 15:10] Кроссплатформенность: вынес Qt AxContainer в условную зависимость только для Windows; исключил AxContainer из общих find_package; добавил MACOSX_BUNDLE в CMake и комментарий по .icns; на Unix-системах перенёс путь `TagTemplate.json` в QStandardPaths::AppDataLocation - Успех
[
2025-09-26 16:10:00] Кроссплатформенность: почистил CMake — удалил ручные флаги -std=c++17 и -fPIC (полагаться на CMAKE_CXX_STANDARD/PIC), убрал дублирующее добавление resources.qrc для Qt5, исключил LinguistTools из find_package (не используется); ресурсы .ico/.jpg убраны из qrc как необязательные для Linux/macOS - Успех
[
2025-09-26 16:30:00] Аудит кроссплатформенности: проверены исходники на WinAPI/ActiveX использование — WinAPI отсутствует; ActiveX подключен условно (WIN32). Проверены пути конфигов: Windows — рядом с EXE; Unix/macOS — AppDataLocation. Проверены ресурсы: .ico не включён в qrc. Предложены улучшения (опции ENABLE_QT_CHARTS, генерация .desktop/.app при install) — ожидает подтверждения - Успех (аудит)