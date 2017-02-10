
#define PROPERTYSTRING(name, code) if (id == (code &0xffff)) {retval = #name;}

#define FOREACH_PROPERTY(CB)			   \
  CB(LayoutTightLayout, 0x08001C00) \
  CB(PageWidth, 0x14001C01) \
  CB(PageHeight, 0x14001C02) \
  CB(OutlineElementChildLevel, 0x0C001C03) \
  CB(Bold, 0x08001C04) \
  CB(Italic, 0x08001C05) \
  CB(Underline, 0x08001C06) \
  CB(Strikethrough, 0x08001C07) \
  CB(Superscript, 0x08001C08) \
  CB(Subscript, 0x08001C09) \
  CB(Font, 0x1C001C0A) \
  CB(FontSize, 0x10001C0B) \
  CB(FontColor, 0x14001C0C) \
  CB(Highlight, 0x14001C0D) \
  CB(RgOutlineIndentDistance, 0x1C001C12) \
  CB(BodyTextAlignment, 0x0C001C13) \
  CB(OffsetFromParentHoriz, 0x14001C14) \
  CB(OffsetFromParentVert, 0x14001C15) \
  CB(NumberListFormat, 0x1C001C1A) \
  CB(LayoutMaxWidth, 0x14001C1B) \
  CB(LayoutMaxHeight, 0x14001C1C) \
  CB(ContentChildNodesOfOutlineElement, 0x24001C1F) \
  CB(ElementChildNodesOfSection, 0x24001C20) \
  CB(EnableHistory, 0x08001E1E) \
  CB(RichEditTextUnicode, 0x1C001C22) \
  CB(ListNodes, 0x24001C26) \
  CB(NotebookManagementEntityGuid, 0x1C001C30) \
  CB(OutlineElementRTL, 0x08001C34) \
  CB(LanguageID, 0x14001C3B) \
  CB(LayoutAlignmentInParent, 0x14001C3E) \
  CB(PictureContainer, 0x20001C3F) \
  CB(PageMarginTop, 0x14001C4C) \
  CB(PageMarginBottom, 0x14001C4D) \
  CB(PageMarginLeft, 0x14001C4E) \
  CB(PageMarginRight, 0x14001C4F) \
  CB(ListFont, 0x1C001C52) \
  CB(TopologyCreationTimeStamp, 0x18001C65) \
  CB(LayoutAlignmentSelf, 0x14001C84) \
  CB(IsTitleTime, 0x08001C87) \
  CB(IsBoilerText, 0x08001C88) \
  CB(PageSize, 0x14001C8B) \
  CB(PortraitPage, 0x08001C8E) \
  CB(EnforceOutlineStructure, 0x08001C91) \
  CB(EditRootRTL, 0x08001C92) \
  CB(CannotBeSelected, 0x08001CB2) \
  CB(IsTitleText, 0x08001CB4) \
  CB(IsTitleDate, 0x08001CB5) \
  CB(ListRestart, 0x14001CB7) \
  CB(IsLayoutSizeSetByUser, 0x08001CBD) \
  CB(ListSpacingMu, 0x14001CCB) \
  CB(LayoutOutlineReservedWidth, 0x14001CDB) \
  CB(LayoutResolveChildCollisions, 0x08001CDC) \
  CB(IsReadOnly, 0x08001CDE) \
  CB(LayoutMinimumOutlineWidth, 0x14001CEC) \
  CB(LayoutCollisionPriority, 0x14001CF1) \
  CB(CachedTitleString, 0x1C001CF3) \
  CB(DescendantsCannotBeMoved, 0x08001CF9) \
  CB(RichEditTextLangID, 0x10001CFE) \
  CB(LayoutTightAlignment, 0x08001CFF) \
  CB(Charset, 0x0C001D01) \
  CB(CreationTimeStamp, 0x14001D09) \
  CB(Deletable, 0x08001D0C) \
  CB(ListMSAAIndex, 0x10001D0E) \
  CB(IsBackground, 0x08001D13) \
  CB(IRecordMedia, 0x14001D24) \
  CB(CachedTitleStringFromPage, 0x1C001D3C) \
  CB(RowCount, 0x14001D57) \
  CB(ColumnCount, 0x14001D58) \
  CB(TableBordersVisible, 0x08001D5E) \
  CB(StructureElementChildNodes, 0x24001D5F) \
  CB(ChildGraphSpaceElementNodes, 0x2C001D63) \
  CB(TableColumnWidths, 0x1C001D66) \
  CB(Author, 0x1C001D75) \
  CB(LastModifiedTimeStamp, 0x18001D77) \
  CB(AuthorOriginal, 0x20001D78) \
  CB(AuthorMostRecent, 0x20001D79) \
  CB(LastModifiedTime, 0x14001D7A) \
  CB(IsConflictPage, 0x08001D7C) \
  CB(TableColumnsLocked, 0x1C001D7D) \
  CB(SchemaRevisionInOrderToRead, 0x14001D82) \
  CB(IsConflictObjectForRender, 0x08001D96) \
  CB(EmbeddedFileContainer, 0x20001D9B) \
  CB(EmbeddedFileName, 0x1C001D9C) \
  CB(SourceFilepath, 0x1C001D9D) \
  CB(ConflictingUserName, 0x1C001D9E) \
  CB(ImageFilename, 0x1C001DD7) \
  CB(IsConflictObjectForSelection, 0x08001DDB) \
  CB(PageLevel, 0x14001DFF) \
  CB(TextRunIndex, 0x1C001E12) \
  CB(TextRunFormatting, 0x24001E13) \
  CB(Hyperlink, 0x08001E14) \
  CB(UnderlineType, 0x0C001E15) \
  CB(Hidden, 0x08001E16) \
  CB(HyperlinkProtected, 0x08001E19) \
  CB(TextRunIsEmbeddedObject, 0x08001E22) \
  CB(ImageAltText, 0x1C001E58) \
  CB(MathFormatting, 0x08003401) \
  CB(ParagraphStyle, 0x2000342C) \
  CB(ParagraphSpaceBefore, 0x1400342E) \
  CB(ParagraphSpaceAfter, 0x1400342F) \
  CB(ParagraphLineSpacingExact, 0x14003430) \
  CB(MetaDataObjectsAboveGraphSpace, 0x24003442) \
  CB(TextRunDataObject, 0x24003458) \
  CB(TextRunData, 0x40003499) \
  CB(ParagraphStyleId, 0x1C00345A) \
  CB(HasVersionPages, 0x08003462) \
  CB(ActionItemType, 0x10003463) \
  CB(NoteTagShape, 0x10003464) \
  CB(NoteTagHighlightColor, 0x14003465) \
  CB(NoteTagTextColor, 0x14003466) \
  CB(NoteTagPropertyStatus, 0x14003467) \
  CB(NoteTagLabel, 0x1C003468) \
  CB(NoteTagCreated, 0x1400346E) \
  CB(NoteTagCompleted, 0x1400346F) \
  CB(NoteTagDefinitionOid, 0x20003488) \
  CB(NoteTagStates, 0x04003489) \
  CB(ActionItemStatus, 0x10003470) \
  CB(ActionItemSchemaVersion, 0x0C003473) \
  CB(ReadingOrderRTL, 0x08003476) \
  CB(ParagraphAlignment, 0x0C003477) \
  CB(VersionHistoryGraphSpaceContextNodes, 0x3400347B) \
  CB(DisplayedPageNumber, 0x14003480) \
  CB(SectionDisplayName, 0x1C00349B) \
  CB(NextStyle, 0x1C00348A) \
  CB(WebPictureContainer2_3_98, 0x200034C8) \
  CB(ImageUploadState, 0x140034CB) \
  CB(TextExtendedAscii, 0x1C003498) \
  CB(PictureWidth, 0x140034CD) \
  CB(PictureHeight, 0x140034CE) \
  CB(PageMarginOriginX, 0x14001D0F) \
  CB(PageMarginOriginY, 0x14001D10) \
  CB(WzHyperlinkUrl, 0x1C001E20) \
  CB(TaskTagDueDate, 0x1400346B)
  //  CB(ContentChildNodesOfPageManifest, 0x24001C1F)
  //  CB(ElementChildNodesOfPage, 0x24001C20)
  //  CB(ElementChildNodesOfTitle, 0x24001C20)
  //  CB(ElementChildNodesOfOutline, 0x24001C20)
  //  CB(ElementChildNodesOfOutlineElement, 0x24001C20)
  //  CB(ElementChildNodesOfTable, 0x24001C20)
  //  CB(ElementChildNodesOfTableRow, 0x24001C20)
  //  CB(ElementChildNodesOfTableCell, 0x24001C20)
  //  CB(ElementChildNodesOfVersionHistory, 0x24001C20)

#define DEFINE_CONSTANTS(NAME, VAL) const uint32_t NAME = VAL;
FOREACH_PROPERTY(DEFINE_CONSTANTS)

#define SWITCH_RETVAL_APPEND(name,constant,v0,v1)  case (constant & 0xffff): retval += #name;break;

#define FOR_EACH_JCID(CB)  \
  CB(jcidPersistablePropertyContainerForTOC, 0x00020001, 0x2D, 0x2E) \
  CB(jcidSectionNode, 0x00060007, 0xA1, 0xA2) \
  CB(jcidPageSeriesNode, 0x00060008, 0xA1, 0xA2) \
  CB(jcidPageNode, 0x0006000B, 0xA1, 0xA2) \
  CB(jcidOutlineNode, 0x0006000C, 0xA1, 0xA2) \
  CB(jcidOutlineElementNode, 0x0006000D, 0xA1, 0xA2) \
  CB(jcidRichTextOENode, 0x0006000E, 0xA1, 0xA2) \
  CB(jcidImageNode, 0x00060011, 0xA1, 0xA2) \
  CB(jcidNumberListNode, 0x00060012, 0xA1, 0xA2) \
  CB(jcidOutlineGroup, 0x00060019, 0xA1, 0xA2) \
  CB(jcidTableNode, 0x00060022, 0xA1, 0xA2) \
  CB(jcidTableRowNode, 0x00060023, 0xA1, 0xA2) \
  CB(jcidTableCellNode, 0x00060024, 0xA1, 0xA2) \
  CB(jcidTitleNode, 0x0006002C, 0xA1, 0xA2) \
  CB(jcidPageMetaData, 0x00020030, 0xA1, 0xA2) \
  CB(jcidSectionMetaData, 0x00020031, 0xA1, 0xA2) \
  CB(jcidEmbeddedFileNode, 0x00060035, 0xA1, 0xA2) \
  CB(jcidPageManifestNode, 0x00060037, 0xA1, 0xA2) \
  CB(jcidConflictPageMetaData, 0x00020038, 0xA1, 0xA2) \
  CB(jcidVersionHistoryContent, 0x0006003C, 0xA1, 0xA2) \
  CB(jcidVersionProxy, 0x0006003D, 0xA1, 0xA2) \
  CB(jcidNoteTagSharedDefinitionContainer, 0x00120043, 0xC0, 0xC0)	\
  CB(jcidRevisionMetaData, 0x00020044, 0xA1, 0xA2)	     \
  CB(jcidVersionHistoryMetaData, 0x00020046, 0xA1, 0xA2) \
  CB(jcidParagraphStyleObject, 0x0012004D, 0xC0, 0xC0)
//  CB(jcidParagraphStyleObjectForText, 0x0012004D, 0xC0, 0xC0)
//  CB(jcidReadOnlyPersistablePropertyContainerForAuthor, 0x00120001, 0xA1, 0xA2)
//  CB(jcidPersistablePropertyContainerForTOCSection, 0x00020001, 0x2D, 0x2E)
#define DEFINE_JCID_CONSTANTS(NAME, VAL, A, B) const uint32_t NAME = VAL;
FOR_EACH_JCID(DEFINE_JCID_CONSTANTS)
