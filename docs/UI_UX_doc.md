# UI/UX Guidelines — PriceTagMaster

## Interaction Model
- Primary flow: Select/drag-and-drop Excel → Preview tags → Edit template → Export DOCX/XLSX.
- Non-blocking parsing with minimal progress indicator; errors as inline banners.

## Design System
- Themes: Light and Dark; toggle in `MainWindow` toolbar/menu.
- Light theme tokens (updated):
  - Canvas `#F8FAFC`, Surface `#FFFFFF`, Subtle `#F1F5F9/#F3F7FF`
  - Text `#0F172A`
  - Border `#E2E8F0` (neutral), Emphasis `#CBD5E1`
  - Primary accent `#2563EB` (blue-600); success `#22C55E` (green-500)
  - Primary gradient (buttons): `#2563EB → #22C55E` (hover `#1D4ED8 → #16A34A`)
  - Focus/selection: palette Highlight `#2563EB`
  - Inputs focus: border `#2563EB`, background `#EEF2FF`
  - Tabs: hover `#F3F7FF`, selected border `#2563EB`
  - GroupBox title color `#2563EB`
- Drop area (Main): default border 2px dashed `#CBD5E1`, hover `#2563EB`, success `#10B981`; radius 12px; default canvas `#F1F5F9`
- Colors: Follow Qt palette for widgets; ensure contrast for disabled states (AC-6).
- Typography: Default system sans-serif; font family/size configurable per template field.
- Spacing: Use mm for geometry; maintain consistent paddings within tags.

## Template Editor (Widget: TemplateEditorDialog)
- Canvas: `QGraphicsView` with mm-to-scene mapping; zoom (Ctrl+wheel), snap-to-grid (optional, later).
- Property Panel:
  - Tag geometry: width/height (mm)
  - Margins and inter-tag spacing (mm)
  - Visible fields and static text blocks
  - Font: family, size (pt), weight, alignment, padding
  - Borders: visibility and stroke
- Presets: starter templates; JSON save/load.

## Preview (Widget: PreviewWidget)
- Pages: A4 portrait 210×297 mm; navigator for multi-page; zoom controls 50–400%.
- Rendering source of truth: shared LayoutEngine (mm) used by both preview and exports.
- Pagination: left-to-right, top-to-bottom; replication per row as configured.

## Accessibility
- Keyboard navigation for template fields list and property edits.
- Focus outlines visible in both themes.
- Minimum contrast ratio for text per WCAG AA when possible within Qt palette constraints.

## Error/Notice Patterns
- Missing headers: non-blocking banner with list of missing names; continue with available data.
- Malformed workbook: modal with remediation tips; keep last valid state in preview.

## Export UX
- Export dialog provides format choice (DOCX/XLSX), destination, and brief summary of current template.
- After export, show path and quick-open.

## Strings and Localization
- Initial UI in English; future localization out-of-scope.
