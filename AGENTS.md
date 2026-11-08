# Project workflow

Create frequent local commits after each small, coherent change. Use very short, clear, lowercase English commit messages. Do not push unless requested. Preserve unrelated user changes.

Before adding or styling any UI control, inspect its equivalent in `/home/ayberk/Projects/remotecode/workbench/src` and `/home/ayberk/Projects/remotecode/ui-kit/src`. Reuse the established design language, including thin scrollbars and square-ended white selection lines. Keep surfaces black and text white. The application must fit the desktop viewport; only dedicated content regions may scroll.

The application is STARLAB. Use the supplied logo at `public/brand/starlab.png` wherever branding or an application icon is required. Use the bundled Michroma font across all controls. Avoid microtext and persistent explanatory/status rows; use intuitive icons, tooltips and inline editing instead.

# Progress tracking

Read `ideas.md` before planning new work and review it after each completed milestone. Record new architectural decisions there. Tick items only after implementation and relevant verification; preserve the distinction between the current demo and planned native/flight integration. Implement our own C++/Python calculation engine; do not embed or delegate production calculations to the OpenRocket Java engine. The authoritative calculation source is /home/ayberk/Projects/B3/roketsimdeneme1. Reuse the existing C++/Python calculations there and preserve their assumptions. Do not implement calculations missing from that source for now; tell the user what is missing. OpenRocket is only an optional supporting reference. This latest source restriction supersedes earlier OpenRocket-first plans.

Keep `ideas`, `ideas.md`, and project documentation local and untracked. Do not commit or publish these notes. Update their checklists locally as work progresses. Preserve required third-party license notices.

After adding, removing, or changing calculation nodes, regenerate the local `CALCS` index with `python3 scripts/update-calcs.py` and verify `python3 scripts/update-calcs.py --check`. Keep the index local and untracked under the existing documentation policy; the generator is maintained source code.
