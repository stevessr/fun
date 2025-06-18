import re

def convert_google_txt_to_markdown(filename="Google.txt"):
    """
    Converts the content of Google.txt (with specific formatting) to Markdown.

    Args:
        filename (str): The path to the Google.txt file.

    Returns:
        str: The Markdown-formatted content.  Returns None if the file
             doesn't exist.
    """
    try:
        with open(filename, "r", encoding="utf-8") as f:
            content = f.read()
    except FileNotFoundError:
        print(f"Error: File '{filename}' not found.")
        return None

    markdown_content = ""

    # --- Helper Functions ---

    def process_noteta(text):
        """Processes the {{noteTA}} template."""
        # Example: {{noteTA |G1=IT |G2=Country |1 = zh-hans:布林; zh-hant:布林;}}
        text = text.replace("{{noteTA", "").replace("}}", "").strip()
        parts = text.split("|")
        #  (Basic conversion, doesn't handle all cases)
        return "" # noteTA is complex, often best left out, or add placeholders.


    def process_about(text):
        """Processes the {{about}} template."""
        parts = text.replace("{{about", "").replace("}}", "").strip().split("|")
        if len(parts) >= 4:
            return f"_This article is about {parts[1]}.  For {parts[2]}, see [{parts[3]}]._"
        elif len(parts) >= 2:
            return f"_This article is about {parts[1]}._"
        return ""

    def process_infobox_company(text):
       """Processes the {{Infobox company}} template."""
       text = text.replace("{{Infobox company", "").replace("}}", "").strip()
       lines = text.split("\n|")  # Split by lines starting with '|'
       infobox_data = {}
       for line in lines:
           if "=" in line:
               key, value = line.split("=", 1)
               infobox_data[key.strip()] = value.strip()
    
       # Build the Markdown table (basic format)
       table_str = ""
       if 'name' in infobox_data:
           table_str += f"## {infobox_data['name']}\n\n"  # Title

       if 'name_en' in infobox_data:
           table_str += f"**{infobox_data['name_en']}**\n\n"

       if 'logo' in infobox_data:
         table_str += f"![{infobox_data.get('name', 'Google Logo')}](https://commons.wikimedia.org/wiki/File:{infobox_data['logo']})\n\n"


       if 'image' in infobox_data:
           table_str += f"![{infobox_data.get('image_caption', 'Google Image')}](https://commons.wikimedia.org/wiki/File:{infobox_data['image']})\n"
           if 'image_caption' in infobox_data:
              table_str += f"*{infobox_data['image_caption']}*\n\n"


       table_str += "| Key               | Value                               |\n"
       table_str += "|-------------------|-------------------------------------|\n"
       for key, value in infobox_data.items():
           if key not in ('name', 'name_en','logo', 'image', 'image_caption'):  # Already handled
               table_str += f"| {key.replace('_', ' ').title()} | {value} |\n"
       table_str += "\n"
       return table_str

    def process_lang(text):
        """Processes {{lang}} template."""
        # {{lang|en|To organize the world's information...}}
        match = re.match(r"{{lang\|(.*?)\|(.*?)}}", text)
        if match:
            lang_code = match.group(1)
            content = match.group(2)
            return f"_{content}_ ({lang_code})"  # Italicize for common usage
        return text

    def process_cite_web(text):
      """Processes the {{cite web}} template (simplified version)."""
      # Example: {{cite web |url=http://... |title=Title |...}}
      text = text.replace("{{cite web", "").replace("}}", "").strip()
      parts = {}
      for item in text.split("|"):
          item = item.strip()
          if "=" in item:
              key, value = item.split("=", 1)
              parts[key.strip()] = value.strip()

      if "url" in parts and "title" in parts:
          return f"[{parts['title']}]({parts['url']})"
      elif "url" in parts:
           return f"[{parts['url']}]({parts['url']})"
      return "" # Return empty string if essential parts missing

    def process_cite_news(text):
        """Processes the {{cite news}} template (simplified)."""
        text = text.replace("{{cite news", "").replace("}}", "").strip()
        parts = {}
        for item in text.split("|"):
            item = item.strip()
            if "=" in item:
                key, value = item.split("=", 1)
                parts[key.strip()] = value.strip()
        if "url" in parts and "title" in parts:
            return f"[{parts['title']}]({parts['url']})"
        elif "url" in parts:
             return f"[{parts['url']}]({parts['url']})"

        return ""

    def process_cite_journal(text):
        """Processes {{cite journal}} (simplified)."""
        return process_cite_web(text)  # Reuse cite web for basic link

    def process_cite_book(text):
        """Processes {{cite book}} (simplified)."""
        return process_cite_web(text)

    def process_cite_press_release(text):
        """Processes {{cite press release}}."""
        return process_cite_web(text)

    def process_ubl(text):
        """Processes the {{ubl}} template (Un বুলেটed List)."""
        items = text.replace("{{ubl", "").replace("}}", "").strip().split("|")
        return ", ".join(items)

    def process_official_url(text):
        """Processes the {{official URL}} template."""
        match = re.match(r"{{official URL\|(.*?)}}", text)
        if match:
            url = match.group(1)
            return f"[Official Website]({url})"
        return text

    def process_start_date_and_age(text):
        """Processes the {{Start date and age}} template."""
        match = re.match(r"{{Start date and age\|(\d+)\|(\d+)\|(\d+)}}", text)
        if match:
            year = match.group(1)
            month = match.group(2)
            day = match.group(3)
            return f"{year}-{month}-{day}"  # Simple YYYY-MM-DD format
        return text

    def process_note_tag(text):
        """Processes {{NoteTag}} (simplified)."""
        return ""  # Usually best to omit, or add a simple footnote marker.

    def process_main(text):
        """Processes {{main}}."""
        articles = text.replace("{{main", "").replace("}}", "").strip().split("|")
        links = [f"[{article}]({article})" for article in articles]
        return "Main articles: " + ", ".join(links)


    def process_see_also(text):
         """Processes {{See also}}."""
         articles = text.replace("{{See also", "").replace("}}", "").strip().split("|")
         links = [f"[{article}]({article})" for article in articles]
         return "See also: " + ", ".join(links)


    def process_link_en(text):
        """Processes {{Link-en}}."""
        match = re.match(r"{{Link-en\|(.*?)\|(.*?)}}", text)
        if match:
           #  return f"[{match.group(1)}](https://en.wikipedia.org/wiki/{match.group(2)})" #Original
           return f"[{match.group(1)}]" #Simplified version, removed en.wikipedia part
        return text


    def process_nasdaq2(text):
        """Processes {{nasdaq2}}."""
        match = re.match(r"{{nasdaq2\|(.*?)}}", text)
        if match:
            return f"`{match.group(1)}` (Nasdaq)"  # Use backticks for code-like display
        return text

    def process_dead_link(text):
        return "" #Remove Dead Link

    def replace_templates(text):
        """Replaces various templates with their Markdown equivalents."""
        text = re.sub(r"{{[^}]+}}", lambda m: process_template(m.group(0)), text)
        return text

    def process_template(text):
        """Processes a single template and calls the appropriate helper."""
        if text.startswith("{{noteTA"):
            return process_noteta(text)
        elif text.startswith("{{about"):
            return process_about(text)
        elif text.startswith("{{Infobox company"):
            return process_infobox_company(text)
        elif text.startswith("{{lang"):
            return process_lang(text)
        elif text.startswith("{{cite web"):
            return process_cite_web(text)
        elif text.startswith("{{cite news"):
            return process_cite_news(text)
        elif text.startswith("{{cite journal"):
            return process_cite_journal(text)
        elif text.startswith("{{cite book"):
            return process_cite_book(text)
        elif text.startswith("{{cite press release"):
            return process_cite_press_release(text)
        elif text.startswith("{{ubl"):
            return process_ubl(text)
        elif text.startswith("{{official URL"):
            return process_official_url(text)
        elif text.startswith("{{Start date and age"):
            return process_start_date_and_age(text)
        elif text.startswith("{{NoteTag"):
            return process_note_tag(text)
        elif text.startswith("{{main"):
            return process_main(text)
        elif text.startswith("{{See also"):
            return process_see_also(text)
        elif text.startswith("{{Link-en"):
            return process_link_en(text)
        elif text.startswith("{{nasdaq2"):
            return process_nasdaq2(text)
        elif text.startswith("{{Dead link"):
            return process_dead_link(text)

        return text  # Return unchanged if no specific handler found.


    # --- Main Conversion Steps ---

    # 1. Replace templates *before* handling other formatting
    content = replace_templates(content)


    # 2. Basic Replacements (Do these after template processing)
    content = content.replace("'''", "**")  # Bold
    content = content.replace("''", "*")    # Italics
    content = content.replace("[[", "[").replace("]]", "]") # Simplify links

    # 3. Handle sections (headers)
    sections = content.split("==")
    markdown_content = ""
    for i, section in enumerate(sections):
        if i == 0:
            markdown_content += section
        elif i % 2 != 0 and i < len(sections)-1:  # Odd sections are titles
            markdown_content += f"\n## {section.strip()}\n" # h2
        else:
            markdown_content += section


    # 4. Remove remaining double square brackets, and other cleanups.
    markdown_content = re.sub(r'\[\[(.*?)\]\]', r'\1', markdown_content)
    markdown_content = re.sub(r'\[\[(.*)\|.*\]\]',r'[\1]',markdown_content) # Remove double brackets
    markdown_content = re.sub(r'\[(http[s]?://\S+)\s+(.*?)\]', r'[\2](\1)', markdown_content)  # External links

    #Clean up extra spaces and lines
    markdown_content = re.sub(r'\n\s*\n', '\n\n', markdown_content) # Remove empty lines with only spaces


    return markdown_content


# --- Example Usage ---

markdown = convert_google_txt_to_markdown("Google.txt")

if markdown:
    with open("Google.md", "w", encoding="utf-8") as outfile:
        outfile.write(markdown)
    print("Conversion complete.  Output saved to Google.md")
