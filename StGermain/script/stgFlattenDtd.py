import xml.sax.handler
import pprint

def readXML( xml_text ):
    # We don't want sax to attempt to load the DTD if listed... remove the line if there.    
    start = xml_text.find( '<!DOCTYPE' )
    if start >= 0:
        end = xml_text[start+1:].find( '>' )
        if end >= 0:
            xml_text = xml_text[:start] + xml_text[start+end+2:]
        # don't bother about else... if DOCTYPE is broken, the whole XML is broken and let it fail below

    p = xml.sax.make_parser()
    h = Handler()
    p.setContentHandler( h )
    p.feed( xml_text )
    p.close()

    return h.getDict()

def getXmlHandler( xml_text ):
    start = xml_text.find( '<!DOCTYPE' )
    if start >= 0:
        end = xml_text[start+1:].find( '>' )
        if end >= 0:
            xml_text = xml_text[:start] + xml_text[start+end+2:]
        # don't bother about else... if DOCTYPE is broken, the whole XML is broken and let it fail below

    p = xml.sax.make_parser()
    h = Handler()
    p.setContentHandler( h )
    p.feed( xml_text )
    p.close()

    return h


class Handler( xml.sax.handler.ContentHandler ):
    def __init__( self ):
        self.inStGermainData = False
        self.stack = []
 
    def startElement( self, name, attributes ):
        if name == "StGermainData":
            if self.inStGermainData == False:
                self.inStGermainData = True
                self.stack.append( {} )
                self._top()["content"] = []
                self._top()["tag"] = name
                self._top()["attr"] = {}
                for attr in attributes.keys():
                    self._top()["attr"].update( { attr: attributes[attr] } )
            else:
                raise RuntimeError( 'Invalid meta xml - parsing tag StGermainData' )
        elif name == "import" or name == "plugins" or name =="components":
            if self.inStGermainData:
                self.stack.append( {} )
                self._top()["content"] = []
                self._top()["tag"] = name
                self._top()["attr"] = {}
                for attr in attributes.keys():
                    self._top()["attr"].update( { attr: attributes[attr] } )
            else:
                raise RuntimeError( 'Invalid meta xml - parsing tag param' )
        elif name == "toolbox":
            if self.inStGermainData:
                self.stack.append( {} )
                self._top()["content"] = ""
                self._top()["tag"] = name
                self._top()["attr"] = {}
                for attr in attributes.keys():
                    self._top()["attr"].update( { attr: attributes[attr] } )
            else:
                raise RuntimeError( 'Invalid meta xml - parsing tag param' )
        elif name == "element":
            if self.inStGermainData:
                self.stack.append( {} )
                # get attributes
                self._top()["attr"] = {}
                for attr in attributes.keys():
                    self._top()["attr"].update( { attr: attributes[attr] } )
                # initializa content according to the type
                if self._top()["attr"]["type"] == 'param':
                    self._top()["content"] = ""
                elif self._top()["attr"]["type"] == 'struct':
                    self._top()["content"] = []
                elif self._top()["attr"]["type"] == 'list':
                    self._top()["content"] = []
                self._top()["tag"] = name
            else:
                raise RuntimeError( 'Invalid meta xml - parsing tag param' )

    def _top( self ):
        return self.stack[ len( self.stack ) - 1 ]


    def _peek( self ):
        return self.stack[ len( self.stack ) - 2 ]


    def _pop( self ):
        if self._peek()["tag"] == "StGermainData":
            self._peek()["content"].append( self._top() )
        elif self._peek()["tag"] == "import" or self._peek()["tag"] == "plugins" or self._peek()["tag"] == "components":
            self._peek()["content"].append( self._top() )
        elif self._peek()["tag"] == "element":
            if self._peek()["attr"]["type"] == "list":
                self._peek()["content"].append( self._top() )
            elif self._peek()["attr"]["type"] == "struct":
                self._peek()["content"].append( self._top() )
        self.stack.pop()


    def characters( self, data ):
        if self._top()["tag"] == "element":
            if self._top()["attr"]["type"] == "param":
                self._top()["content"] += data
        elif self._top()["tag"] == "toolbox":
                self._top()["content"] += data
 
    def endElement( self, name ):
        if name == "StGermainData":
            self.inStGermainData = 0
        else:
            self._pop()

    def getDict( self ):
        if len( self.stack ):
            return self.stack

    def updateDict( self, stack ):
        self.stack[0]['content'].extend( stack )

    def writeXml( self ):
        txt = '<?xml version="1.0"?>\n' 
        txt += '<!DOCTYPE StGermainData SYSTEM "stgermain.dtd">\n'
        if len( self.stack ):
            for stack in self.stack:
                txt = self.recurseXml( stack, txt )
        return txt

    def recurseXml( self, stacks, txt ):
        if stacks['tag'] == 'element':
            if stacks['attr']['type'] == 'param':
                txt = txt + '<' + stacks['tag']
                for attr in stacks['attr'].keys():
                    txt = txt + ' ' + attr + '="' + stacks['attr'][attr] + '"'
                txt = txt + '>'
                txt += stacks['content']
                txt += '</' + stacks['tag'] + '>\n'
                return txt
            else:
                txt = txt + '<' + stacks['tag']
                for attr in stacks['attr'].keys():
                    txt = txt + ' ' + attr + '="' + stacks['attr'][attr] + '"'
                txt = txt + '>\n'
        elif stacks['tag'] == 'toolbox':
            txt = txt + '<' + stacks['tag']
            for attr in stacks['attr'].keys():
                txt = txt + ' ' + attr + '="' + stacks['attr'][attr] + '"'
            txt = txt + '>'
            txt += stacks['content']
            txt += '</' + stacks['tag'] + '>\n'
            return txt
        else:
            txt = txt + '<' + stacks['tag']
            for attr in stacks['attr'].keys():
                txt = txt + ' ' + attr + '="' + stacks['attr'][attr] + '"'
            txt = txt + '>\n'
        for stack in stacks['content']:
            txt = self.recurseXml( stack, txt )

        txt = txt + '</' + stacks['tag'] + '>\n'
                
        return txt

    # stgermain argument container
    class StgArg:
        def __init__( self, stgArg ):
            self.stgArg = stgArg
            try:
                if '=' in stgArg:
                    self.value =( "=".join( stgArg.split("=")[1:] )).replace('\\', '')
                else:
                    self.value = None
            except Exception:
                self.value = None
            self.fields = (stgArg.split("=")[0]).split('.')

        def get_length( self ):
            return len( self.fields )

        def is_list( self, fieldNumber ):
            fieldSplit = self.fields[ fieldNumber ].split( '[' )
            if len( fieldSplit ) > 1:
                return True
            else:
                return False
        
        def get_list_number( self, fieldNumber ):
            return int( self.fields[ fieldNumber ].split( '[' )[1].split( ']' )[0] )

        def is_new_list( self, fieldNumber ):
            return '[]' in self.fields[ fieldNumber ]

        def get_name( self, fieldNumber ):
            if self.is_list( fieldNumber ):
                return self.fields[ fieldNumber ].split( '[' )[0]
            else:
                return self.fields[ fieldNumber ]
       
    """
    recurse_modify_param checks whether it is the last one in the argument or not before
    moving on to check other things. 

    If it is the last one, then we have to either create, or change the current param. 
    If it is not the last one, we check for the same things as well, whether we are creating,
    or recursing more.

    Things that was taken into consideration is whether it is a struct, or a list.

    the modify_param was tested with these input with the flatten xml of gsv_milestone5

    ( 'components.context.Type=test')
    ( 'maxTimeSteps=3' )
    ( 'components.plotWindowREF' )
    ( 'components.plotWindowREF.Type=lucDefaultWindow' )
    ( 'components.plotWindowREF.RenderingEngine=renderingEngine' )
    ( 'components.plotWindowREF.OutputFormat[]=imageOutput' )
    ( 'plugins[3].DrillCores[].numSamples=10' )
    ( 'plugins[3].DrillCores[0].drawCapture=drillCoreCapture' )
    ( 'plugins[3].DrillCores[0].radius=200' )
    ( 'temperatureBCs.vcList[0].variables[0].value=291.15' )

    """
    def recurse_modify_param( self, stgArg, index, stack ):
        name = stgArg.get_name( index )
        if stgArg.get_length() == index + 1:
            for var in stack:
                if var['attr'].has_key( 'name' ):
                    if var['attr']['name'] == name:
                        if var['attr']['type'] == 'param':
                            var['content'] = stgArg.value
                            return
                        else:
                        # if it is not a param, it must be either list or struct
                            if stgArg.is_new_list( index ):
                                if stgArg.value != None:
                                    param = {}
                                    param['tag'] = 'element'
                                    param['attr'] = {}
                                    param['attr']['type'] = 'param'
                                    param['content'] = stgArg.value

                                    var['content'].append( param )
                                    return
                else:
                    if name == 'components':
                        # TODO
                        pass
                    elif name == 'plugins' or name == 'import':
                        # TODO
                        pass
            # if it gets here, means it is new, since it is outside for loop
            self.modify_param_new_last( stgArg, index, stack )
            return
        else:
            # not last, so we recurse
            for var in stack:
                if var['attr'].has_key( 'name' ):
                    if var['attr']['name'] == name:
                        if stgArg.is_new_list( index ):
                            # new list here, since it is not last, it adds a struct
                            struct = {}
                            struct['tag'] = 'element'
                            struct['content'] = []
                            struct['attr'] = {}
                            struct['attr']['type'] = 'struct'
                            var['content'].append( struct )
                            self.recurse_modify_param( stgArg, index+1, var['content'][ len( var['content'] ) - 1 ]['content'] )
                        
                        elif stgArg.is_list( index ):
                            self.recurse_modify_param( stgArg, index+1, var['content'][ stgArg.get_list_number( index ) ]['content'] )
                        else:
                            self.recurse_modify_param( stgArg, index+1, var['content'] )
                        return
                else:
                    if name == 'components':
                        if name == var['tag']:
                            self.recurse_modify_param( stgArg, index+1, var['content'] )
                            return
                    elif name == 'plugins' or name == 'import':
                        if name == var['tag']:
                            if stgArg.is_new_list( index ):
                                # TODO: if there are no plugins or import being specified, and create new list
                                pass
                            elif stgArg.is_list( index ):
                                self.recurse_modify_param( stgArg, index+1, var['content'][ stgArg.get_list_number( index ) ]['content'] )
                            return
            # if it gets here, means it is new
            #self.modify_param_new_not_last( stgArg, index, stack )
            return


    def modify_param_new_not_last( self, stgArg, index, stack ):
        # if it cannot find, means there can be couple probability
        # if it is a new list
        # or a struct
        if stgArg.is_new_list( index ):
            ls = {}
            ls['tag'] = 'element'
            ls['content'] = []
            ls['attr'] = {}
            ls['attr']['name'] = stgArg.get_name( index )
            ls['attr']['type'] = 'list'

            struct = {}
            struct['tag'] = 'element'
            struct['content'] = []
            struct['attr'] = {}
            struct['attr']['type'] = 'struct'
            ls['content'].append( struct )
            stack.append( ls )
        else:
            struct = {}
            struct['tag'] = 'element'
            struct['content'] = []
            struct['attr'] = {}
            struct['attr']['name'] = stgArg.get_name( index )
            struct['attr']['type'] = 'struct'
            stack.append( struct )

    def modify_param_new_last( self, stgArg, index, stack ):
        if stgArg.value == None:
            # if value == none, it is either a list, or a new struct
            if stgArg.is_new_list( index ):
                param = {}
                param['tag'] = 'element'
                param['content'] = []
                param['attr'] = {}
                param['attr']['name'] = stgArg.get_name( index )
                param['attr']['type'] = 'list'
                stack.append( param )
            else:
                param = {}
                param['tag'] = 'element'
                param['content'] = []
                param['attr'] = {}
                param['attr']['name'] = stgArg.get_name( index )
                param['attr']['type'] = 'struct'
                stack.append( param )
        else:
            if stgArg.is_new_list( index ):
                ls = {}
                ls['tag'] = 'element'
                ls['content'] = []
                ls['attr'] = {}
                ls['attr']['name'] = stgArg.get_name( index )
                ls['attr']['type'] = 'list'

                param = {}
                param['tag'] = 'element'
                param['content'] = stgArg.value
                param['attr'] = {}
                param['attr']['type'] = 'param'
                ls['content'].append( param )
                stack.append( ls )
            else:
                # otherwise, we create a new param
                param = {}
                param['tag'] = 'element'
                param['content'] = stgArg.value
                param['attr'] = {}
                param['attr']['name'] = stgArg.get_name( index )
                param['attr']['type'] = 'param'
                stack.append( param )


            
    def modify_param( self, stgArg ):
        arg = self.StgArg( stgArg )
        stack = self.stack[0]['content']
        self.recurse_modify_param( arg, 0, stack )

