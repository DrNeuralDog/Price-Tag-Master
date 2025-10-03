 [REVERTED 2025-10-03] Запись снята: изменения отменены по запросу пользователя, возвращено предыдущее поведение Excel-генерации
[2025-10-02 16:20:00] Пользователь попросил заменить дефолт 9900 на "Цена: " и прижать влево - Принято, обновлён дефолт текста и выравнивание в include/tagtemplate.h
[2025-10-02 16:35:00] Пользователь уточнил: менять нужно только в генераторах, а редактор шаблонов оставить как было - Принято, дефолты в include/tagtemplate.h откатил; генераторы Excel/Word теперь всегда выводят слева "Цена: " без опоры на шаблон
[2025-09-30 21:45:00] Запрос пользователя: рефакторинг src/configmanager.cpp без изменения логики - Принято к исполнению
[2025-09-30 10:00:00] Запрос на рефакторинг ExcelParser::parseDataRow без изменения логики - Выполнено: созданы хелперы и упрощён метод
[2025-09-30 19:06:31] Ошибка сборки после рефакторинга ExcelParser - Исправлено: методы findMaxDataRow/parseDataRow/validatePriceTag помечены как const
[2025-09-25 00:00] Запрос от пользователя: исправить формат DOCX ценников (после 3-й строки теги тоньше) - Начата работа над фиксацией макета таблиц Word
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
[2025-09-26 16:10:00] User asked for Linux/macOS readiness; confirmed removal of Windows-specific flags and optional resources; CMake streamlined for cross-platform - Applied

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
[2025-09-22 00:00:00] User requested 2mm spacing for toolbar buttons and bottom line - Implemented via QToolBar padding in updateThemeStyles (src/mainwindow.cpp)
[2025-09-22 00:10:00] User reported buttons still at top - Wrapped theme/lang buttons with 2mm top/bottom margins and added QToolBar QToolButton vertical margins for gear action - Applied in src/mainwindow.cpp
[2025-09-22 00:20:00] User reported toolbar bottom line disappears on theme toggle - Removed toolbar padding; apply only border-bottom and toolbutton margins in updateThemeStyles - Fixed
[2025-09-22 00:25:00] User reported gear (template editor) button became too small - Restored min clickable size 28x28 via QToolBar QToolButton min-width/min-height - Fixed
[2025-09-22 00:35:00] User requested gear icon to match lang size and click area not oversized - Applied exact 28x28 icon and fixed size via widgetForAction(QToolButton), removed extra padding/border on QToolBar QToolButton - Fixed
[2025-09-22 00:50:00] User requested larger gear icon like before - Cropped transparent margins from PNG and set gear button to 32×32 for parity; icon updated on theme change - Applied
[2025-09-22 01:05:00] User requested gear icon 1.25× larger than 42px and darker background to match UI - Set 52×52 icon/button and made background transparent to blend with interface - Applied
[2025-09-22 01:12:00] User reported toolbar/button background mismatch with UI - Set QToolBar background to palette(window) and forced transparent QToolButton background to fully match surrounding UI - Fixed
[2025-09-22 01:25:00] User requested reduce vertical clickable area and avoid right edge stick - Added toolbar right margin 2mm, wrapped gear button with 1mm top/bottom margins around 52×52, ensured no extra padding - Applied
[2025-09-22 02:00:00] User reported gear button clickable area exceeds icon bounds - Applied alpha-based mask from icon to QToolButton; hit area now equals icon shape; mask updates on theme change - Success
[2025-09-22 02:08:00] User reported gear button disappeared after mask - Reverted to normal rectangular hit area (clearMask), kept 52×52 size and cropped icon for visuals - Success
[2025-09-22 02:15:00] User requested to trim clickable area on sides only - Implemented eventFilter to accept clicks only in central rect (left/right trimmed by 2mm) without using masks - Success
[2025-09-22 02:25:00] User reported no change with eventFilter - Implemented custom QToolButton overriding hitButton(); trimmed left/right by 4mm in logical DPI; added as toolbar widget bound to action - Success
[2025-09-25 01:20] Запрос от пользователя: в XLSX заменить ведущий пробел на 'П' в текстах из редактора шаблонов - Выполнено
[2025-09-25 01:28] Запрос от пользователя: заменить 'П' на прозрачные '*' в XLSX - Выполнено
[2025-09-25 01:50] Запрос от пользователя: добавить кнопку сброса формата ценников до значений по умолчанию (в окне настроек, снизу слева, RU/EN, сохранять в TagTemplate.json) - Реализовано: добавлена кнопка Reset в TemplateEditorDialog; сбрасывает шаблон к дефолту, обновляет превью и сохраняет TagTemplate.json
[2025-09-25 02:05] Запрос от пользователя: первая попытка DnD не принимается — убрать первичный блок - Исправлено: добавлен dragMoveEvent, теперь перетаскивание принимается сразу над зоной сброса
[2025-09-26 14:05] Запрос от пользователя: dnd зона показывает красный запрет и принимает файл только после 5 попыток - Изменено поведение: dragEnter теперь сразу принимает валидные .xlsx, визуальный hover включается только при наведении над `dropArea`; dragMove переключает стиль/accept в зависимости от позиции
[2025-09-30 19:25] Запрос от пользователя: увеличить размер кнопки смены языка (RU/EN не помещается) - Выполнено: ширина кнопки увеличена, текст RU/EN отображается полностью
[
2025-09-30 20:05:30] Пользователь сообщил: в светлой теме текст в кнопке смены языка обрезается (видна лишь середина) - Исправлено: убран отрицательный паддинг и зафиксированы размеры через QSS в `styleLanguageButton` - Успех
[2025-09-30 20:18:00] Пользователь сообщил: проблема сохраняется - Приняты доп. меры: добавлен objectName=lang, селекторы `QPushButton#lang` в локальном стиле и в Light QSS ThemeManager; установлены fixedSize(40,28) и нулевой горизонтальный padding - Применено
[2025-09-26 14:06] Запрос подтверждения: выполнить git commit с правками DnD и обновлёнными логами - Ожидается подтверждение
[2025-09-30 00:00:00] Запрос пользователя: рефакторинг `MainWindow::updateCharts()` без изменения логики - Выполнено, метод разбит на подметоды; линтер чисто
[2025-09-30 00:20:00] Запрос пользователя: рефакторинг остальных методов `mainwindow.cpp` - Выполнено: разбиты `setupToolbar`, DnD-обработчики, `showStatistics`; поведение не изменено
[2025-09-30 00:45:00] Запрос пользователя: рефакторинг `excelparser.cpp` - Выполнено: `parseExcelFile()` разбит на хелперы; логика без изменений
[2025-09-30 00:55:00] Запрос пользователя: рефакторинг `findHeaders` в `excelparser` - Выполнено: разобран на хелперы, поведение сохранено
[2025-09-26 15:10] Пользователь запросил аудит кроссплатформенности (Linux/macOS): выявлен безусловный AxContainer и Windows-ресурсы; подготовлены правки CMake (AxContainer только на WIN32, MACOSX_BUNDLE hook) и перенос пути конфига в AppData на Unix - В процессе
[2025-09-26 16:30] Запрос от пользователя: проверить препятствия сборки под Linux/macOS и предложить замены - Выполнено аудит: WinAPI в проекте не используется; ActiveX (AxContainer) подключается только на WIN32; конфиги на Unix/macOS — через QStandardPaths; ресурсы .ico не тянутcя в Unix. Предложены улучшения: (1) отключить сборку Xlsx DOCX/ZIP приватных хедеров без лишних include на публичных API; (2) добавить ENABLE_QT_CHARTS опцию; (3) генерацию .app и .desktop в Install шаге - Ожидает подтверждения
[2025-09-26 13:51] Пользователь: ошибки сборки Qt5 MinGW по типам QtCharts (QChartView/QPieSeries) - Внесены правки: квалифицировал QtCharts:: в заголовке и ограничил using в updateCharts(); готово к пересборке
[
2025-09-29 17:12:30] Пользователь: несовместимость Qt5/Qt6 — 'QtCharts' does not name a type; падение MOC - Применено: поля-члены QChartView заменены на QWidget* в `include/mainwindow.h`; локальные QChartView создаются в `updateCharts()` и присваиваются; `using namespace QtCharts` теперь только под Qt5. Уточнён CMake с USE_QT_CHARTS. Ожидается пересборка
[2025-09-30 21:05:00] Запрос пользователя: рефакторинг ExcelGenerator::generateExcelDocument без изменения логики - Выполнено: метод разбит на хелперы (createTagFormats, renderTag, write*Row); добавлена оркестрация; логика и вывод идентичны. Удалён #if 0 блок старой реализации
[2025-10-01 15:35:00] Запрос пользователя: установить PriceTagMangerIcon.ico как иконку приложения для отображения на панели задач Windows - Выполнено: добавлен .ico файл в ресурсы Qt, обновлен app_icon.rc для использования ресурса вместо внешнего пути
[2025-10-01 19:35:00] Пользователь сообщил об ошибке сборки Windows ресурсов - Исправлено: изменен путь в app_icon.rc с Qt resource syntax на файловый путь для корректной работы windres.exe
[2025-10-01 19:40:00] Пользователь сообщил о повторной ошибке поиска файла иконки - Исправлено: указан относительный путь ../resources/icons/PriceTagMangerIcon.ico от build директории к исходным файлам
[2025-10-01 19:45:00] Пользователь сообщил о повторной ошибке поиска файла иконки - Исправлено: изменен путь на ../../resources/icons/PriceTagMangerIcon.ico для корректного доступа из build/Debug директории
[2025-10-01 19:50:00] Пользователь сообщил о повторной ошибке поиска файла иконки - Применен абсолютный путь к файлу иконки в app_icon.rc для надежной работы windres.exe независимо от рабочей директории
[2025-10-02 14:30:00] Запрос пользователя: структурировать CMakeLists.txt файл с четкими разделителями формата ===================================================================================================================== для лучшей читаемости - Выполнено: файл переструктурирован с ясными разделами для базовых настроек, зависимостей, конфигурации проекта и платформенных особенностей
[2025-10-02 15:00:00] Пользователь сообщил об ошибках сборки после реструктуризации CMakeLists.txt - Исправлено: перемещен блок конфигурации цели после её создания, теперь проект конфигурируется без ошибок
[2025-10-02 17:05:00] Пользователь сообщил: Excel-генератор переносит последние строки ценника на новую страницу - Выполнено: пересчитана сетка по реальным высотам строк и убран мнимый вертикальный/горизонтальный spacing; добавлен safety 0.5 мм для предотвращения надломов строк
[
2025-10-03 18:26:36] Пользователь сообщил: ошибка сборки windres — ICON: No such file or directory (resources/windows/app_icon.rc) - Исправлено: путь к иконке заменён на относительный `../../resources/icons/PriceTagManagerIcon.ico`; пересборка прошла успешно