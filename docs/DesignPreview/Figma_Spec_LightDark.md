## Figma спецификация — Light & Dark Themes (PriceTagMaster)

### 1) Токены темы
- Источник: `docs/Figma_Tokens_LightDark.json` (совместимо с Figma Tokens/Variables)
- Наборы: `global`, `light`, `dark`, `components`

### 2) Страницы Figma
1. UI Kit — Foundations
   - Color: Swatches из токенов `global`, `light`, `dark`
   - Typography: `fontFamilies.ui`, `fontSizes`, `lineHeights`
   - Spacing/Radii/Shadows: из `global`
2. Components
   - Buttons: Primary, Secondary, Ghost, Destructive (states: default/hover/pressed/disabled)
   - Inputs: TextField (label/helper/error), Combobox
   - Toolbar: App toolbar + Theme toggle
   - Panels: Surface/Property Panel
   - Banners: Info/Warning/Error
3. Screens
   - Main (Empty state): Dropzone с plus-button
   - Main (With file): Preview + right Property Panel
   - Template Editor: Canvas (`QGraphicsView`), свойства, zoom
   - Export Dialog: формат (DOCX/XLSX), путь, summary

### 3) Компоненты и варианты
- Button (AutoLayout, paddingX=`components.button/paddingX`, height=`components.button/height`, radius=`components.button/radius`)
  - Variants: kind=[Primary, Secondary, Ghost, Destructive], state=[Default, Hover, Pressed, Disabled], theme=[Light, Dark]
  - Цвета:
    - Light/Primary: bg `{global.color.brand/primary}`, text `light.text/inverse`
    - Dark/Primary: bg `{global.color.brand/primary}`, text `light.text/inverse`
    - Secondary: border `light|dark.border.strong`, text `...text/primary`, bg `...bg/surface`
- TextField
  - States: Default/Focus/Error/Disabled
  - Tokens: border `...border/default|strong`, focus `...focus`, text `...text/primary`, placeholder `...text/secondary`
- Toolbar
  - Height=`components.toolbar/height`, bg `...bg/surface`, divider `...border/default`
  - ThemeToggle: иконка луны/солнца, hit area 32x32, hover/pressed по токенам
- Panel
  - bg `...bg/surface`, radius=`components.panel/radius`, shadow=`global.shadow.elevation/1`
  - Header: title md/semibold, sub `text/secondary`
- Banner
  - Info/Warning/Error: bg `...highlight/bg` или специальные (info/success/warning/error), иконка 16px, border `...border/strong`

### 4) Экраны — layout и аннотации
- Main (Empty)
  - Canvas bg: `...bg/canvas`
  - Dropzone: dashed border `...border/strong`, text `...text/secondary`, plus-button = Primary
- Main (With file)
  - Left: Preview (A4 page mock): page card bg `#FFFFFF` (оба режима), тень `global.shadow.elevation/2`, внутренний отступ=`components.preview/pagePadding`
  - Right: Property Panel: поля для margins/spacing/tag size/replication
- Template Editor
  - Canvas: серый плаш `...bg/subtle`, ruler опционально, zoom чипы справа снизу
- Export Dialog
  - Two-radio (DOCX/XLSX), path input, summary капшн, Primary CTA

### 5) Доступность и контраст
- Минимум WCAG AA: контраст текста к фону ≥ 4.5:1 для `text/primary`, ≥ 3:1 для `text/secondary`
- Фокус-стиль: 2px outline `...focus` с offset 2px

### 6) Гайд по импортy в Figma
1) Установить плагин “Figma Tokens” или включить Variables
2) Импортировать `Figma_Tokens_LightDark.json` как Token Set/Variables
3) Создать страницу “UI Kit — Foundations”, разместить палитры/типы/шаги
4) Создать компоненты с Variant properties (kind/state/theme) и связать цвета с токенами
5) Создать страницы “Components” и “Screens”, собрать примеры экранов

### 7) Мэппинг на Qt Widgets
- Palette.Window = `...bg/surface` | `...bg/canvas`
- Palette.Base = `...bg/surface`
- Palette.Text/WindowText = `...text/primary`
- Disabled: текст `...disabled/text`, bg `...disabled/bg`
- Focus: outline цвет = `...focus`
- Dark: усиливать divider до `...border/strong`

### 8) Примечания
- Размеры в токенах — pt/px для UI; для Template/Layout в коде — мм (см. PRD §6). Пагинация и геометрия остаются источником истины в движке.

