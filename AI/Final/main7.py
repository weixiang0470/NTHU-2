# 新增的依賴庫
from flask import Flask, request, abort
from linebot import LineBotApi, WebhookHandler
from linebot.exceptions import InvalidSignatureError
from linebot.models import MessageEvent, TextMessage, TextSendMessage, ImageSendMessage
import tempfile
import os
import uuid
from myutility import *
from myCarousel import *
import warnings
from langdetect import detect

warnings.filterwarnings("ignore")

# Global constant for storing images
STATIC_IMAGE_DIR = os.path.join(os.path.dirname(__file__), "static/temp_images")
os.makedirs(STATIC_IMAGE_DIR, exist_ok=True)
ngrok_url = "https://8600-140-114-87-235.ngrok-free.app"
# 初始化 Line Bot
app = Flask(__name__, static_folder='static')
# 我
# line_bot_api = LineBotApi("e8/fJt3I/LlN+rXOEEpNvHRlGZzodfKrH/EZDVrkccE0Mhn/1AdyeuVDQ2l1E35945Tn+diI+YShPwU7NXFB2ArgPEtv400gjIYjiIrzSSdEElpCXmjWt5l2xVtHCp6V9Uq9MbPC/bBNvFlJk3BRTAdB04t89/1O/w1cDnyilFU=")
# handler = WebhookHandler("0e8d8c8c4bf39c7b08d0d395e025f82c")
# 沈
line_bot_api = LineBotApi("O+Sevsc7QxILyCjeoroEuEgZiZCKjPiBntk50y3XKpqaBa6RkJ0b+YGB8aOJCIyI0vLg4l75riklnbQoD//XiBQUvFKZjlT86dW0C46uJwMO7knpL3OOnRGHsuWNNLyvYUWnWpKebfBVoLmDa/5GowdB04t89/1O/w1cDnyilFU=")
handler = WebhookHandler("b8bea39f85ea857a4bbdb9dcaa3be3ef")


USER_Floor = {}
optimizer = AdOptimizer()
# client = genai.Client(api_key='AIzaSyBbhK6AmFH7D2ODSbzmY__7h3fnz__yPc0')
# best_img = None
feedback_log = []
user_sessions={}

@app.route("/callback", methods=['POST'])
def callback():
    signature = request.headers['X-Line-Signature']
    body = request.get_data(as_text=True)
    print("callback")
    try:
        handler.handle(body, signature)  # 驗證簽名並處理事件
        print("handling signature.\n")
    except InvalidSignatureError:
        abort(400)
    return 'OK'

@handler.add(MessageEvent, message=TextMessage)
def handle_message(event):
    print("user input = ",event.message.text)
    handle_text_message(event)  # 將文字訊息交給處理類別

def generate_image(prompt: str, style: str = "photographic", creativity: float = 0.5):
    response = client.models.generate_content(
        model="gemini-2.0-flash-preview-image-generation",
        contents=prompt,
        config=types.GenerateContentConfig(response_modalities=['TEXT','IMAGE'])
    )
    return response

def generate_and_send_image(event, session, user_id):
    print("generate_and_send_image")
    try:        
        theme = session['data']['theme']
        style = session['data']['style']
        elements = [e.strip() for e in session['data']['element'].split(',')]

        # Use logic-based agent to build the ad prompt
        agent = PromptAgent(KnowledgeBase())
        user_lang = session.get('lang', 'en')
        logic_prompt = agent.generate_prompt({
            "themes": [theme],
            "styles": [style],
            "elements": elements,
            "content": []
        })

        print(f"Logic-based prompt:\n{logic_prompt}")

        # Create base Gemini-compatible prompt
        modified_prompt, params, session['action_id'] = optimizer.select_action(
            theme,
            style,
            ', '.join(elements),
            logic_prompt
        )

        final_prompt = modified_prompt


        print(final_prompt)
        image_data = generate_image(final_prompt, **params)
        print("step 3")
        generated_img = extract_image_from_response(image_data)
        print("step 4")
        generated_img.show()

        image_url = upload_temp_image(generated_img)
        reply_image(event, image_url, user_id)
        # session['state'] = 'wait_feedback'
        return generated_img

    except Exception as e:
        line_bot_api.push_message(
            user_id,
            TextSendMessage(text=f"Error generating, please retry...\n{e}")
        )
        # session['state'] = 'start'
        # generate_and_send_image(event, session, user_id)
        return None
    
def reply_image(event, image_url, user_id):
    print("reply_image")
    try:
        line_bot_api.push_message(
            user_id,
            ImageSendMessage(
                original_content_url=image_url,
                preview_image_url=image_url
            )
        )
    except Exception as e:
        line_bot_api.reply_message(
            event.reply_token,
            TextSendMessage(text="圖片發送失敗，請稍後再試")
        )
        raise

def upload_temp_image( image):
    print("upload_temp_image")
    filename = f"{uuid.uuid4().hex}.png"
    print(f'filename: {filename}')
    image_path = os.path.join(STATIC_IMAGE_DIR, filename)
    print(f'image_path: {image_path}')
    image.save(image_path, format="PNG")
    print(f'This is our file: {filename}')
    return f"{ngrok_url}/static/temp_images/{filename}"

def edit_image(image: Image.Image, edit_instruction: str) -> Image.Image:
    # Gemini expects PIL Image directly, no need for bytes conversion or custom types
    contents = [
        f"Please edit the image as follows: {edit_instruction}",
        image
    ]
    print("editting image ....")
    response = client.models.generate_content(
        model="gemini-2.0-flash-preview-image-generation",
        contents=contents,
        config=types.GenerateContentConfig(response_modalities=['TEXT', 'IMAGE'])
    )

    print(response.text)
    print(f"Edit prompt = {contents}")

    for part in response.candidates[0].content.parts:
        if part.inline_data is not None:
            edited_image = Image.open(BytesIO(part.inline_data.data))
            return edited_image
    return None

def handle_text_message(event):
    # global best_img
    print("handle_text_message")
    user_id = event.source.user_id
    text = event.message.text

    if user_id not in user_sessions:
        kb = KnowledgeBase()
        user_sessions[user_id] = {
            'state': 'start',
            'data': {},
            'iteration': 0,
            'max_iterations': 5,
            'best_score': 0,
            'feedback_log': [],
            'kb': kb

        }

    session = user_sessions[user_id]
    print(user_id, text)
    print(session)
    
    

    state = session['state']
    
    # if user_id not in USER_Floor:
    #     print("\n\nnot in floor\n\n")
    #     USER_Floor[user_id] = 0
    #     start(event)

    text=text.lower()
    text=text.replace('，',',')
    
    if state == 'start':
        print("s = start")
        text=text.replace(' ','')
        if text != 'start':
            line_bot_api.reply_message(
                event.reply_token,
                TextSendMessage(text="Please enter 'start' to begin generating a green ad.")
            )
            return
        session['state'] = 'get_theme'
        line_bot_api.reply_message(
            event.reply_token,
            TextSendMessage(text="Please enter a theme for your ad (e.g. ocean, forest, recycling, climate action).")
        )

    elif state == 'get_theme':
        print("s = theme")
        session['data']['theme'] = text
        session['state'] = 'get_style'
        user_lang = detect(text)  # returns 'en', 'zh-tw', 'zh-cn', etc.
        session['lang'] = user_lang
        line_bot_api.reply_message(
            event.reply_token,
            TextSendMessage(text="Please enter the style you want (e.g. illustration, watercolor, photography, 3d-rendering).")
        )

    elif state == 'get_style':
        print("s = style")
        session['data']['style'] = text
        session['state'] = 'get_element'
        line_bot_api.reply_message(
            event.reply_token,
            TextSendMessage(text="Please enter the elements you want to appear (separated by commas, e.g. turtle, fish, trash).")
        )

    elif state == 'get_element':
        print("s = element")
        session['data']['element'] = text

        session['image_origin'] = generate_and_send_image(event, session, user_id)
        # if image_origin:
        if session['image_origin']==None:return
        print("generating img")
        line_bot_api.reply_message(
            event.reply_token,
            TextSendMessage(text="Rate (1-5) this image? Do you want to edit this image? (yes/no): (ex: 2, yes)")
        )
        session['state'] = 'edit'
        return

    
    elif state == 'edit':
        print("s = edit")
        # if ',' not in text:
        #     line_bot_api.reply_message(
        #         event.reply_token,
        #         TextSendMessage(text="請用正確格式輸入，例如：2, yes")
        #     )
        #     return
        text=text.replace(' ','')
        try:
            score, edit_or_not = [s.strip() for s in text.split(',')]
            score = int(score)
            if int(score) >= user_sessions[user_id]['best_score']:
                user_sessions[user_id]['best_score'] = int(score)
                session['best_img']= session['image_origin']
            print("score = ",score," edit? ",edit_or_not)
            if edit_or_not == "yes":
                line_bot_api.reply_message(
                    event.reply_token,
                    TextSendMessage(text="Describe how you'd like to edit the image:")
                )
                session['state'] = 'editting'   

            elif edit_or_not == "no":
                line_bot_api.reply_message(
                    event.reply_token,
                    TextSendMessage(text="Please enter 'end' to exit.")
                )
                session['state'] = 'wait_feedback'
        except Exception as e:
            line_bot_api.push_message(
                user_id,
                TextSendMessage(text=f"Wrong format, Please use the format: \"number, yes/no\". eg. \"4, no\" {e}")
            )
            return
    
    elif state =='editting':
        print("s = editting")
        edited_img = edit_image(session['image_origin'], text)
        if edited_img:
            session['image_origin'] = edited_img
            edited_img.show()
            image_url = upload_temp_image(edited_img)
            print(f'image has been uploaded to {image_url}')
            reply_image(event, image_url, user_id)
            line_bot_api.push_message(
                user_id,
                TextSendMessage(text="Rate (1-5) & Comment the edited image:(ex: 4, this image is good) ")
            )

            session['state'] = 'model_updating'   

        else:
            print(f"Error during image generation: {...}")
    
    elif state == 'model_updating':
        print("s = model_updating")
        try:
            edited_score, edited_comment = text.split(',')
            optimizer.update_model(session['action_id'], int(edited_score))

            feedback_log.append({
                'score': edited_score,
                'comment': edited_comment,
                'image_data': session['image_origin']  # You could store edited image separately if needed
            })

            if int(edited_score) >= user_sessions[user_id]['best_score']:
                user_sessions[user_id]['best_score'] = int(edited_score)
                session['best_img'] = session['image_origin']
            
            line_bot_api.reply_message(
                event.reply_token,
                TextSendMessage(text="Do you want to edit this image? (yes/no):")
            )

            session['state'] = 'edit_loop'  

        except Exception as e:
            line_bot_api.push_message(
                user_id,
                TextSendMessage(text=f"Wrong format, Please use the format: \"number, comment\". eg. \"4, this image is good\" {e}")
            )
            return

    elif state =='edit_loop':
        print("s = edit_loop")
        text=text.replace(' ','')
        if text == "yes":
            line_bot_api.reply_message(
                event.reply_token,
                TextSendMessage(text="Describe how you'd like to edit the image:")
            )
            session['state'] = 'editting'   
            
        elif text == "no":
            line_bot_api.reply_message(
                event.reply_token,
                TextSendMessage(text="Please enter 'end' to exit.")
            )
            session['state'] = 'wait_feedback'
    
    elif state == 'wait_feedback':
        print("s = wait_feedback")
        text=text.replace(' ','')
        if text.lower() == 'end':
            

            line_bot_api.reply_message(
                event.reply_token,
                TextSendMessage(text="This is the best image you chose.")
            )
            image_url = upload_temp_image(session['best_img'])
            print(f'image has been uploaded to {image_url}')
            reply_image(event, image_url, user_id)
            line_bot_api.push_message(
                user_id,
                TextSendMessage(text="Thank you for using it.")
            )
            session['data'] = {}
            session['best_score'] = 0 
            session['best_img'] = None
            session['state'] = 'start'
        else:
            line_bot_api.reply_message(
                event.reply_token,
                TextSendMessage(text="Please enter 'end' to exit.")
            )

def start(event):
    global USER_Floor
    message = "enter \'start\' to start"#Carousel_template()
    #line_bot_api.reply_message(event.reply_token, message)
    
    UserId = event.source.user_id
    #Fs = ['聊天', '文字轉語音', '天氣預報', '圖片人物分析']
    #line_reply = '你想幹嘛呢 : '
    #for i in range(len(Fs)):
    #    line_reply += f'\n{i+1}.{Fs[i]}'
    USER_Floor[UserId] = 1
    #line_reply += '\n(輸入"End"結束)'
    line_bot_api.reply_message(
                    event.reply_token,
                    TextSendMessage(text=message)
                )
    # line_bot_api.reply_message(event.reply_token, message)

if __name__ == "__main__":
    # line_bot_handler = LineBotHandler()
   
    app.run(port=5000)