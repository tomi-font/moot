- Entities getting their composition modified will still be unchanged in later callbacks of the same frame.

	- Which is wanted and needed for the Systems.
	- But could cause unwanted behavior in callbacks?
	- The handle could be curated before being sent to callbacks.
	- This might not ever matter.
