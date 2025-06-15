import numpy as np
from google import genai
from PIL import Image
from io import BytesIO  
import random
from typing import List
from typing import List, Set, Dict
import yaml



client = genai.Client(api_key='GOOGLE_API')

def generate_gemini_sustainable_prompt(
    theme: str,
    style: str,
    user_elements: List[str],
    additional_details: str = None
) -> str:
    """
    Generate a Gemini-optimized image generation prompt by combining user input with automatic sustainability elements.

    :param theme:            Main subject or concept (e.g., 'community event').
    :param style:            Desired visual art style (e.g., 'digital painting').
    :param user_elements:    User-specified elements to include in the scene (not necessarily sustainable).
    :param additional_details: Optional extra instructions.
    :return:                 A complete prompt string suitable for Gemini image generation.
    """

    sustainability_elements_pool = [
        "solar panels on rooftops",
        "wind turbines in the background",
        "recycling bins and composting stations",
        "people using reusable containers and utensils",
        "urban gardens and community green spaces",
        "electric bikes or scooters",
        "rainwater collection barrels",
        "eco-friendly market stalls made of reclaimed wood",
        "children learning about nature",
        "clean rivers and green walkways"
    ]
    sustainability_elements = [
        "wind turbines and solar panels",
        "rooftop gardens and vertical farms",
        "recycling bins and composting stations",
        "bike lanes and electric buses",
        "people planting trees and harvesting vegetables",
        "children learning about nature outdoors",
        "animals living peacefully in city spaces",
        "eco-friendly buildings made from recycled materials",
        "clean rivers, forests, and ocean coastlines",
        "green rooftops and rainwater collection systems"
    ]

    selected_sustainability = random.sample(sustainability_elements_pool, k=random.randint(2, 4))

    prompt_lines = [
        f"A detailed scene of {theme}. (weight=7.0)",
        "Include the following visual elements with weight = 2.0:",
        *[f"- {element}" for element in user_elements],
        *[f"- {s_element}" for s_element in selected_sustainability],
        f"Visual style: {style}."
    ]

    if additional_details:
        prompt_lines.append(f"Additional instructions: {additional_details} (weight 4.0)")

    return "\n".join(prompt_lines)


class AdOptimizer:
    def __init__(self):
        self.prompt_modifications = [
            lambda t,s,e,d: f"\
            theme:{t}\nstyle={s}\nuser_elements={e}\nadditional_details={d}",
            # lambda p: f"Minimalist style, {p}, with green environmental themes",
            # lambda p: f"Powerful eco-ad, {p}, urging action",
            # lambda p: f"Creative green ad, {p}, with sustainability themes"
        ]

        self.style_options = ["photographic", "illustration", "3d-rendering", "watercolor"]
        self.creativity_options = np.linspace(0.3, 0.9, 5)

        self.action_values = np.zeros(len(self.prompt_modifications) * len(self.style_options) * len(self.creativity_options))
        self.action_counts = np.zeros_like(self.action_values)

    def select_action(self, prompt: str,s:str,e:str,d:str):
        epsilon = 0.2
        if np.random.random() < epsilon:
            mod_idx = np.random.randint(len(self.prompt_modifications))
            style_idx = np.random.randint(len(self.style_options))
            creativity_idx = np.random.randint(len(self.creativity_options))
        else:
            best_idx = np.argmax(self.action_values)
            mod_idx = best_idx % len(self.prompt_modifications)
            remaining = best_idx // len(self.prompt_modifications)
            style_idx = remaining % len(self.style_options)
            creativity_idx = remaining // len(self.style_options)

        new_prompt = self.prompt_modifications[mod_idx](prompt,s,e,d)
        params = {
            "style": self.style_options[style_idx],
            "creativity": self.creativity_options[creativity_idx]
        }

        action_id = mod_idx + style_idx * len(self.prompt_modifications) + \
                    creativity_idx * len(self.prompt_modifications) * len(self.style_options)

        return new_prompt, params, action_id

    def update_model(self, action_id: int, reward: float):
        self.action_counts[action_id] += 1
        n = self.action_counts[action_id]
        current_value = self.action_values[action_id]
        self.action_values[action_id] = current_value + (reward - current_value) / n


def extract_image_from_response(response) -> Image.Image:
    """
    Extracts the first image from a Gemini response.
    :param response: The Gemini response object.
    :return: An Image object if an image is found, otherwise None.
    """
    for part in response.candidates[0].content.parts:
        if part.inline_data is not None:
            return Image.open(BytesIO(part.inline_data.data))
    return None

# Logic-based agent to generate prompts for green advertisements
# === Propositional Logic Base +++
class Proposition:
    def __init__(self, name: str):
        self.name = name
    def __repr__(self):
        return self.name
    def __hash__(self):
        return hash(self.name)
    def __eq__(self, other):
        return isinstance(other, Proposition) and self.name == other.name

# === Factories for logical categories +++
def Theme(t): return Proposition(f"Theme_{t}")
def element(m): return Proposition(f"element_{m}")
def Style(s): return Proposition(f"Style_{s}")
def Message(msg): return Proposition(f"Msg_{msg}")

# === Knowledge Base of rules +++
class KnowledgeBase:
    def __init__(self, rule_file: str = "logic_rules.yaml"):
        self.rules: List[(Set[Proposition], str, int)] = []  # (conditions, result, priority)
        self.inferred_facts = set()
        self.load_rules_from_yaml(rule_file)

    def load_rules_from_yaml(self, path: str):
        with open(path, 'r', encoding='utf-8') as file:
            raw_rules = yaml.safe_load(file)
            for rule in raw_rules:
                conditions = set()
                for cond in rule.get("conditions", []):
                    for k, v in cond.items():
                        if k == "Theme":
                            conditions.add(Theme(v))
                        elif k == "Style":
                            conditions.add(Style(v))
                        elif k == "element":
                            conditions.add(element(v))
                        elif k == "Message":
                            conditions.add(Message(v))
                        elif k == "Category":
                            conditions.add(Proposition(f"Category_{v}"))
                priority = rule.get("priority", 0)
                if "result" in rule:
                    self.rules.append((conditions, rule["result"], priority))
                if "inferred" in rule:
                    # Support single key-value or list of pairs
                    if isinstance(rule["inferred"], dict):
                        for k, v in rule["inferred"].items():
                            self.inferred_facts.add(Proposition(f"{k}_{v}"))
                    elif isinstance(rule["inferred"], list):
                        for inferred_item in rule["inferred"]:
                            for k, v in inferred_item.items():
                                self.inferred_facts.add(Proposition(f"{k}_{v}"))


                # Apply inference if specified
                if "inferred" in rule:
                    for k, v in rule["inferred"].items():
                        self.inferred_facts.add(Proposition(f"{k}_{v}"))

        

    def add_rule(self, conditions: List[Proposition], result: str):
        self.rules.append((set(conditions), result))

    def infer_prompt_elements(self, inputs: Set[Proposition], lang: str = 'en') -> List[str]:
        inferred = set(inputs)
        inferred.update(self.inferred_facts)

        result_field = 'result_zh' if lang.startswith('zh') else 'result'
        results = []

        for rule in self.rules:
            conds, result, priority = rule
            if conds.issubset(inferred):
                if isinstance(rule, dict):
                    results.append(rule.get(result_field, rule.get('result')))
                else:
                    results.append(result)

        return results

# === Prompt Assembly Agent +++
class PromptAgent:
    def __init__(self, kb: KnowledgeBase):
        self.kb = kb

    def generate_prompt(self, user_input: Dict[str, List[str]], lang: str = 'en') -> str:
        props = set()
        for t in user_input.get("themes", []):
            props.add(Theme(t))
        for m in user_input.get("elements", []):
            props.add(element(m))
        for s in user_input.get("styles", []):
            props.add(Style(s))
        for msg in user_input.get("content", []):
            props.add(Message(msg))

        elements = self.kb.infer_prompt_elements(props, lang=lang)

        prompt = "寫一則綠色廣告，內容包括：\n" if lang.startswith('zh') else "Write a green advertisement that:\n"
        for el in elements:
            prompt += f"- {el}\n"
        return prompt


# === Main and Rule Base ===
if __name__ == "__main__":
    kb = KnowledgeBase()
    
    # Test
    user_input = {
        "themes": ["ocean", "trees", "community"],
        "elements": ["recycling", "solar panels", "biking"],
        "styles": ["photographic", "illustration"],
        "content": ["save the planet", "reduce waste", "community action"]
    }

    agent = PromptAgent(kb)
    final_prompt = agent.generate_prompt(user_input, lang='zh')
    print(final_prompt)

