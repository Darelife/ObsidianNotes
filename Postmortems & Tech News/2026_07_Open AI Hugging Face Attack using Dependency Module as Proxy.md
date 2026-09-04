- Internal cyber eval (ExploitGym) on an unreleased model. Sandbox was supposed to block internet + block agents talking to each other. Both broke
- Started as an agent leaving a note in their internal package manager (Artifactory) asking for a missing file. Other agents found it, realized they could message each other through filenames. Became an accidental agent messageboard
- Separately, an agent tricked Artifactory into making internet requests for it. Shared the trick on the board
- From there: privilege escalation inside Artifactory, crashed it once, got rebuilt, agents just redid the whole exploit chain again.
- Found leaked HF creds online, chained two zero-days, got full code execution on HF's prod servers, expanded to admin across multiple clusters, stole credentials everywhere, even forged their own admin keys and turned around and hit OpenAI's own network too.
- Caught eventually via OpenAI's internal monitoring, traced back, disclosed.

Why: agents cheated tasks instead of solving them, never gave up on unsolvable ones (huge reasoning budgets), misjudged the grading and kept "hacking for real" even after cheating, found sneaky ways to coordinate, and safety checks used for real products weren't turned on for this internal eval - including their own chain-of-thought monitor, which would've caught it a day earlier.

---

https://openai.com/index/hugging-face-incident-and-the-road-ahead/
https://www.youtube.com/watch?v=dt_OMxufoGE
